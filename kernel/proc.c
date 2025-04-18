/**
 * 
 * Process management for WINIX
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/

#include <kernel/kernel.h>
#include <kernel/table.h>
#include <winix/mm.h>
#include <winix/srec.h>
#include <winix/welf.h>
#include <winix/list.h>
#include <winix/bitmap.h>

// Linked lists are defined by a head and tail pointer.

// Process table
PRIVATE struct proc _proc_table[NUM_PROCS + NUM_TASKS];

// Pointer to proc table, public system wise
PUBLIC struct proc *proc_table;

// Scheduling queues
PUBLIC struct proc *ready_q[NUM_QUEUES][2];

// The currently-running process
PUBLIC struct proc *curr_scheduling_proc;

PUBLIC struct proc *curr_syscall_caller;


/**
 * How is the process image aligned?
 *
 * Each process image is aligned as shown below
 * 
 *              
 * Inaccessible Page (1024 words)   <- rbase
 * Stack (1024 words)               
 * Text segment
 * Data segment
 * Bss segment
 * Heap data                        <- heap_bottom
 *              
 * In the struct proc, rbase points to the first page for which 
 * the process does not have access. This is because NULL points to 0, which is 
 * effectively the first page of the process image. By setting the first page
 * as inaccessible, derefercing NULL will triger page fault.
 * 
 * Heap_bottom points to the end of the process image where memory can be accessed.
 * Heap can be extended by extending heap_bottom.
 * 
 * Stack_top points to the start of the memory where memory can be accessed
 */

/**
 * Report all the running procs to serial port 1
 * 
 *
**/
void kreport_all_procs(struct filp* file) {
    struct proc *curr;
    filp_kprint(file, "PID PPID PG RBASE      PC         STACK      HEAP       PROTECTION    FLAG NAME    \n");

    foreach_proc(curr){
        kreport_proc(curr, file);
    }
}

/**
 * Report the proc's info
**/
void kreport_proc(struct proc* curr, struct filp* file) {
    int ptable_idx = PADDR_TO_PAGED(curr->ctx.rbase)/32;
    filp_kprint(file, "%-3d %-4d %-2d 0x%08lx 0x%08lx 0x%08lx 0x%08lx %d 0x%08lx 0x%03x %s\n",
            curr->pid,
            get_proc(curr->parent)->pid,
            curr->procgrp,
            (uintptr_t)curr->ctx.rbase,
            (uintptr_t)get_physical_addr(get_pc_ptr(curr),curr),
            (uintptr_t)get_physical_addr(curr->ctx.m.sp,curr),
            (uintptr_t)curr->heap_break,
            ptable_idx,
            curr->ctx.ptable[ptable_idx],
            curr->state,
            curr->name);
}

/**
 * get next free pid
**/
pid_t get_next_pid(){
    static pid_t pid = 2;
    if (pid == INT_MAX)
        pid = 2;
    return pid++;
}

/**
 * Gets a pointer to a process by pid
 * if multiple process has the same pid, the first
 * one is returned
 *
 * Parameters:
 *   pid               The process's pid'.
 *
 * Returns:            The relevant process, or NULL if it can't be found
 **/
struct proc* get_proc_by_pid(pid_t pid){
    struct proc* curr;
    if(pid == 0)
        return SYSTEM_TASK;
    foreach_proc(curr){
        if(curr->pid == pid){
            return curr;
        }
    }
    return NULL;
}

/**
 * Gets a pointer to a process.
 *
 * Parameters:
 *   proc_nr        The process to retrieve.
 *
 * Returns:            The relevant process, or NULL if it does not exist.
 **/
struct proc *get_proc(int proc_nr) {
    struct proc* who;
    if (IS_PROCN_OK(proc_nr))
        if(IS_INUSE(who = proc_table + proc_nr))
            return who;
    return NULL;
}

/**
 * similar to get_proc(), but this one makes sure the 
 * returning proc is runnable
 * @param  proc_nr 
 * @return         
 */
struct proc *get_non_zombie_proc(int proc_nr){
    struct proc *p = get_proc(proc_nr);
    if(p && !(p->state & STATE_ZOMBIE))
        return p;
    return NULL;
}

/**
 * Adds a proc to the tail of a list.
 *
 * Parameters:
 *   q        An array containing a head and tail pointer of a linked list.
 *   proc    The proc struct to add to the list.
 **/
void enqueue_tail(struct proc **q, struct proc *proc) {
    ASSERT(!proc->next && q[TAIL] != proc);
    if (q[HEAD] == NULL) {
        q[HEAD] = q[TAIL] = proc;
    }
    else {
        q[TAIL]->next = proc;
        q[TAIL] = proc;
    }
    proc->next = NULL;
}

/**
 * Adds a proc to the head of a list.
 *
 * Parameters:
 *   q        An array containing a head and tail pointer of a linked list.
 *   proc    The proc struct to add to the list.
 **/
void enqueue_head(struct proc **q, struct proc *proc) {
    ASSERT(!proc->next && q[HEAD] != proc);

    if (q[HEAD] == NULL) {
        proc->next = NULL;
        q[HEAD] = q[TAIL] = proc;
    }
    else {
        proc->next = q[HEAD];
        q[HEAD] = proc;
    }
}

/**
 * Removes the head of a list.
 *
 * Parameters:
 *   q        An array containing a head and tail pointer of a linked list.
 *
 * Returns:
 *   The proc struct that was removed from the head of the list
 *   NULL if the list is empty.
 **/
struct proc *dequeue(struct proc **q) {
    struct proc *p = q[HEAD];

    if (p == NULL)
        return NULL;

    if (q[HEAD] == q[TAIL]) { // Last item
        q[HEAD] = q[TAIL] = NULL;
    }
    else { // At least one remaining item
        q[HEAD] = p->next;
    }
    p->next = NULL;
    return p;
}

/**
 * remove the process from the scheduling queue
 * @param  h process to be removed
 * @return   0 on success, -1 if not
 */
int dequeue_schedule( struct proc *h) {
    struct proc *curr;
    struct proc *prev = NULL;
    struct proc ** q = ready_q[h->priority];

    curr = q[HEAD];

    while (curr != h && curr != NULL) {
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL)
        return -EINVAL;

    if (prev == NULL) {
        q[HEAD] = curr->next;
        if(curr->next == NULL){
            q[TAIL] = NULL;
        }
    } else {
        prev->next = curr->next;
    }
    return 0;
}

/**
 * enqueue the process to the scheduling queue
 * according to its priority
 * @param p 
 */
void enqueue_schedule(struct proc* p) {
    enqueue_tail(ready_q[p->priority], p);
}

/**
 * zombify the process, the process will be removed from the
 * ready_q, and memory will be released by the system
 * @param p 
 */
void zombify(struct proc *p){
    p->state |= STATE_ZOMBIE;
    dequeue_schedule(p);
}

/**
 * add this struct to the free_proc list. This method should
 * only be called when a zombie process is released
 * @param p 
 */
void release_zombie(struct proc *p){
    if(p->state & STATE_ZOMBIE){
        p->flags = 0;
        p->pid = 0;
        p->state = -1;
    }
}

/**
 * get a free struct proc from the system proc table
 * @return pointer to the free slot, or NULL
 */
struct proc *get_free_proc_slot() {
    int i;
    struct proc *who;
    for(i = INIT; i <= NUM_PROCS; i++){
        who = &proc_table[i];
        if(!IS_INUSE(who)){
            proc_set_default(who);
            who->state = STATE_RUNNABLE;
            who->flags = IN_USE;
            who->pid = get_next_pid();
            return who;
        }
    }
    return NULL;
}

/**
 * set the process struct to default values
 * @param p 
 */
void proc_set_default(struct proc *p) {
    int pnr_bak = p->proc_nr;
    memset(p, 0, sizeof(struct proc));
    p->proc_nr = pnr_bak;
    p->state = -1;

    memset(p->ctx.m.regs, -1, NUM_REGS * sizeof(reg_t));
    p->ctx.cctrl = DEFAULT_CCTRL;

    p->quantum = DEFAULT_USER_QUANTUM;
    p->ctx.ptable = p->protection_table;
    p->timer.proc_nr = p->proc_nr;
    p->priority = DEFAULT_PRIORITY;
    p->umask = 022;
}

/**
 * allocate stack for kernel processes or kernel thread
 * stack size is defined by KERNEL_STACK_SIZE
 * this method can be used for creating kernel process 
 * or kernel threads'  stack
 * @param       who 
 * @return      virtual address of the stack
 */
reg_t* alloc_kstack(struct proc *who, int size){
    reg_t *addr, *stack_top;

    if(size % PAGE_LEN == 0){
        stack_top = (reg_t*)get_free_pages(size * sizeof(char), GFP_HIGH);
    }else{
        stack_top = (reg_t*)kmalloc(size, sizeof(char));
    }

    addr = stack_top + size - 1;
    *stack_top = STACK_MAGIC;
    who->stack_top = (ptr_t*)stack_top;
    return addr;
}

/**
 * set corressponding fields of struct pro
 */
void set_proc(struct proc *p, void (*entry)(), const char *name) {
    p->ctx.m.pc = entry;
    strlcpy(p->name, name, PROC_NAME_LEN - 1);
}

/**
 * The first page in the kernel is set as unaccessible
 * Remember that NULL is just a macro of a pointer pointing at 0
 *  #define NULL    (void *)0
 * Thus when NULL pointer is dereferenced (is written), 
 * program attemps to write value to address 0, we would set
 * the first page unaccessible, 
 * so that GFP is raised when null pointer is dereferenced
 * 
 */
 void kset_ptable(struct proc* who){
    if(IS_KERNEL_PROC(who)){
        bitmap_fill((unsigned int *)who->ctx.ptable, PTABLE_LEN);
        bitmap_clear_bit((unsigned int *)who->ctx.ptable, PTABLE_LEN, 0);
    }
}

/**
 * Creates a new kernel process and adds it to the runnable queue
 *
 * Parameters:
 *   entry        A pointer to the entry point of the new process.
 *   priority    The scheduling priority of the new process.
 *   name        The name of the process, up to PROC_NAME_LEN characters long.
 *
 * Returns:
 *   The newly-created proc struct.
 *   NULL if the priority is not valid.
 *   NULL if the process table is full.
 *
 * Side Effects:
 *   A proc is removed from the free_proc list, reinitialised, and added to ready_q.
 */
struct proc *start_kernel_proc(struct boot_image* task) {
    struct proc *who = proc_table + task->proc_nr;
    // void (*entry)();
    // int proc_nr;
    // const char *name; 
    // int quantum;
    // int priority;

    proc_set_default(who);
    who->flags |= IN_USE;

    set_proc(who, task->entry, task->name);
    kset_ptable(who);
    who->quantum = task->quantum;
    who->ctx.m.sp = alloc_kstack(who, task->stack_size);
    who->text_top = who->mem_start = (ptr_t*)PAGE_LEN;
    who->text_size = &TEXT_END - &TEXT_BEGIN;
    who->stack_size = task->stack_size;
    who->priority = task->priority;
    who->pid = 0;
    who->state = STATE_RUNNABLE;
    enqueue_schedule(who);
    
    return who;
}

// /**
//  * start a new user process
//  * @param  lines    array containing the binary image of the process
//  * @param  length   length of the lines
//  * @param  entry    entry point of the process
//  * @param  priority 
//  * @param  name     
//  * @return          
//  */
// struct proc *start_user_proc(size_t *lines, size_t length, size_t entry, int options, const char *name){
//     struct proc *p;
//     if(p = get_free_proc_slot()){
//         if(exec_proc(p,lines,length,entry,options,name) == 0)
//             return p;
//     }
//     return NULL;
// }

/**
 * process memory control, 
 * @param  who       
 * @param  page_addr the virtual address memory
 * @param  has_access     whether proc has access to this virtual address
 * @return           
 */
int proc_memctl(struct proc* who ,vptr_t* page_addr, bool has_access){
    int paged = PADDR_TO_PAGED(get_physical_addr(page_addr, who)); // get page descriptor
    int (*func)(unsigned int *map, int map_len,int start) = has_access ? bitmap_set_bit : bitmap_clear_bit;
    
    return func((unsigned int *)who->ctx.ptable, PTABLE_LEN, paged);
}

bool validate_welf(struct winix_elf* elf){
    if(elf->magic != WINIX_ELF_MAGIC)
        return false;
    if(elf->binary_size != elf->text_size + elf->data_size)
        return false;
    return true;
}

bool peek_mem_welf(struct winix_elf* elf, int stack_size, int heap_size){
    int td_aligned = align_page(elf->binary_size + elf->bss_size);
    int proc_len = stack_size + td_aligned + heap_size;
    return (bool)(peek_free_pages(proc_len, GFP_NORM) >= 0);
}

int alloc_mem_welf(struct proc* who, struct winix_elf* elf, int stack_size, int heap_size){
    int vm_offset = elf->binary_offset;
    int proc_len;
    int td_aligned;
    int data_residual;
    ptr_t *bss_start;
    ptr_t* mem_start;

    if (!validate_welf(elf))
        return -EINVAL;

    td_aligned = align_page(elf->binary_size + elf->bss_size);
    proc_len = td_aligned + heap_size;
    mem_start = user_get_free_pages(who, proc_len, GFP_NORM);
    if(mem_start == NULL)
        return -ENOMEM;
    // klog("welf alloc ret 0x%x, len %d\n", mem_start, proc_len);
    // _kreport_bitmap(who->ctx.ptable, MEM_MAP_LEN, kprintf2);
    // _kreport_memtable(kprintf2);

    who->ctx.rbase = (reg_t*)(mem_start - vm_offset);
    who->mem_start = mem_start;
    who->text_top = mem_start;

    who->stack_top = user_get_free_pages(who, stack_size, GFP_HIGH);
    *((reg_t*)who->stack_top) = STACK_MAGIC;
    who->ctx.m.sp = (reg_t*)get_virtual_addr(who->stack_top + stack_size - 1, who);
    memset(who->stack_top, 0, stack_size);

    // set bss segment to 0
    bss_start = mem_start + elf->binary_size;
    memset(bss_start, 0, elf->bss_size);
    
    data_residual = td_aligned - (int)(elf->binary_size + elf->bss_size);
    who->heap_top = bss_start + elf->bss_size;
    who->heap_break = who->heap_top;
    who->heap_bottom = who->heap_break + heap_size + data_residual - 1;
    memset(who->heap_top, 0, heap_size);

    who->data_size = elf->data_size;
    who->text_size = elf->text_size;
    who->bss_size = elf->bss_size;
    who->rbase_offset = elf->binary_offset;
    who->stack_size = stack_size;

    // kdebug("%d alloc from %p to %p\n", who->pid, mem_start, who->heap_bottom);
    return 0;
}

int copy_from_user(struct proc* who, void *dest, vptr_t *src, size_t len){
    ptr_t* p;
    if (!is_vaddr_ok(src, len, who))
        return -EFAULT;
    p = get_physical_addr(src, who);
    memcpy(dest, p, len);
    return len;
}

int copy_to_user(struct proc* who, vptr_t *dest, void *src, size_t len){
    ptr_t* p;
    if (!is_vaddr_ok(dest, len, who))
        return -EFAULT;
    p = get_physical_addr(dest, who);
    memcpy(p, src, len);
    return len;
}


/**
 * Copy values onto the user stack, this is very similar to memcpy
 * @param  who 
 * @param  src 
 * @param  len 
 * @return     
 */
int copyto_user_stack(struct proc *who, void *src, size_t len){
    reg_t *sp = (reg_t*)get_physical_addr(who->ctx.m.sp,who);
    sp -= len;
    memcpy(sp,src,len);
    who->ctx.m.sp = (reg_t*)get_virtual_addr(sp,who);
    return 0;
}

vptr_t* copyto_user_heap(struct proc* who, void *src, size_t len){
    vptr_t* addr = get_virtual_addr(who->heap_break, who);
    if(who->heap_break + len >= who->heap_bottom){
        return NULL;
    }
    memcpy(who->heap_break, src, len);
    who->heap_break += len;
    // kdebug("copy to proc %d heap len %d ret %p\n", who->pid, len, addr);
    return addr;
}

void task_exit(){
    zombify(curr_scheduling_proc);
    wramp_syscall(WINIX_RESCHEDULE); // random number, just to trigger rescheduling
}


/**
 * Initialises the process table
 *
 * Side Effects:
 *   ready_q is initialised to all empty queues.
 *   free_proc queue is initialised and filled with processes.
 *   proc_table is initialised to all DEAD processes.
 *   curr_scheduling_proc is set to NULL.
 **/
void init_proc() {
    int i, procnr_offset;
    struct proc *p;
    int preset_pnr;
    // Initialise queues

    for (i = 0; i < NUM_QUEUES; i++) {
        ready_q[i][HEAD]  = ready_q[i][TAIL] = NULL;
    }

    procnr_offset = NUM_TASKS - 1;
    // Add all proc structs to the free list
    for ( i = 0; i < NUM_PROCS + NUM_TASKS; i++) {
        p = &_proc_table[i];
        proc_set_default(p);
        preset_pnr = i - procnr_offset;
        p->proc_nr = preset_pnr;
    }

    proc_table = _proc_table + procnr_offset;
    curr_scheduling_proc = NULL;
}



