/**
 * Syscall in this file: fork
 * Input:    
 *
 * Return:     reply_res: child's pid to parent,
 *                 and 0 to child
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:08:30
 * 
*/
#include <kernel/kernel.h>
#include <winix/bitmap.h>
#include <winix/mm.h>
#include <winix/list.h>

/**
 * @brief create a new stack in child and copy stack from to child
 *          with virtial address referenced
 * 
 * @param parent 
 * @param child 
 * @return int 
 */
int copy_stack(struct proc* parent, struct proc* child){
    ptr_t* new_stack, *sp_physical, *old_stack;
    unsigned long *stack_ptr, *stack_bottom;
    unsigned long vstack_top, vstack_bottom, val;
    vptr_t* vsp_relative_to_stack_top, *vir_old_stack;
    reg_t **child_sp;
    int i;
    
    new_stack = user_get_free_page(child, GFP_HIGH);
    if(new_stack == NULL){
        release_proc_slot(child);
        return -ENOMEM;
    }
    copy_page(new_stack, parent->stack_top);
    child_sp = &child->ctx.m.sp;
    vsp_relative_to_stack_top = (vptr_t*)(get_physical_addr(parent->ctx.m.sp, parent) - parent->stack_top);
    sp_physical = (ptr_t *)(new_stack + (unsigned long)vsp_relative_to_stack_top) ;
    *child_sp = (reg_t *)get_virtual_addr(sp_physical, child);
    child->stack_top = new_stack;

    stack_ptr = (unsigned long*)child->stack_top;
    stack_bottom = (unsigned long*)stack_ptr + child->stack_size;

    old_stack = parent->stack_top;
    vir_old_stack = get_virtual_addr(old_stack, parent);
    vstack_top = (unsigned long)vir_old_stack;
    vstack_bottom = (unsigned long)vir_old_stack + parent->stack_size;

    // change the virtual address referencing old stack to new stack
    while (stack_ptr < stack_bottom) {
        val = *stack_ptr;
        if (val >= vstack_top && val < vstack_bottom) {
            *stack_ptr = (unsigned long)get_virtual_addr(val - vstack_top + new_stack, child);
            // kdebug("%p: old %lx new %lx\n", (void *)get_virtual_addr(stack_ptr, child), val, *stack_ptr);
        }
        stack_ptr++;
    }
    for (i = 0; i < REGS_NR; i++){
        val = child->ctx.m.regs[i];
        if (val >= vstack_top && val < vstack_bottom) {
            child->ctx.m.regs[i] = (unsigned long)get_virtual_addr(val - vstack_top + new_stack, child);
            // kdebug("reg %d: old %lx new %x\n", i+1, val, child->ctx.m.regs[i]);
        }
    }

    // kdebug("tfork %p %p for %d tp %d\n", (void *)new_stack, (void *)*sp, child->proc_nr, child->thread_parent);
    proc_memctl(child, vir_old_stack, false);
    return 0;
}

/**
 * copy the pcb struct from parent to child
 * @param  parent 
 * @param  child  
 * @return        
 */
int copy_pcb(struct proc* parent, struct proc* child){
    int pbak, i;
    struct filp* file;
    pid_t pidbak;
    pbak = child->proc_nr;
    pidbak = child->pid;
    memcpy(child, parent, sizeof(struct proc));
    child->proc_nr = pbak;
    child->pid = pidbak;
    // child's pending signals are cleared
    child->sig_pending = 0;
    // ptable points to its own protection table
    child->ctx.ptable = child->protection_table;
    child->time_used = child->sys_time_used = 0;

    INIT_LIST_HEAD(&child->pipe_reading_list);
    INIT_LIST_HEAD(&child->pipe_writing_list);

    for (i = 0; i < OPEN_MAX; ++i) {
        file = child->fp_filp[i];
        if(file){
            file->filp_count += 1;
        }
    }
    return 0;
}

/**
 * allocate new virtual address space for chlid, and 
 * copy the whole process image from parent to child
 * @param  parent 
 * @param  child  
 * @return        
 */
int copy_mm(struct proc* parent, struct proc* child){
    ptr_t *src, *dest;

    bitmap_clear((unsigned int *)child->ctx.ptable, PTABLE_LEN);
    child->mem_start = dup_vm(parent, child);
    if(child->mem_start == NULL)
        return -ENOMEM;
    child->text_top = child->mem_start;
    child->ctx.rbase = (reg_t*)(child->mem_start - child->rbase_offset);

    src = (ptr_t *)parent->mem_start;
    dest = (ptr_t *)child->mem_start;
    while(src < parent->heap_bottom){
        copy_page(dest, src);
        src += PAGE_LEN;
        dest += PAGE_LEN;
    }
    return 0;
}


/**
 * copy physical registers. Physical registers are internally used by the kernel
 * Unlike virtual registers like $pc, $ra, some kernel space pointers point to the
 * actually physical memory for fast access
 * @param  parent 
 * @param  child  
 * @return        
 */
void copy_pregs(struct proc* parent, struct proc* child){
    child->message = (struct message *)get_physical_addr(get_virtual_addr(parent->message, parent), child);
    child->heap_break = get_physical_addr(get_virtual_addr(parent->heap_break, parent), child);
    child->heap_bottom = get_physical_addr(get_virtual_addr(parent->heap_bottom, parent), child);
}




/**
 * syscall for fork
 * @param  parent 
 * @return        pid of the child, or -1 if forking is failed
 */
int sys_fork(struct proc *parent) {
    struct proc *child;
    int ret;
    // int tdb_page_len, sp_heap_page_len, page;

    if ((child = get_free_proc_slot())) {
        copy_pcb(parent,child);
        
        if((ret = copy_mm(parent, child))){
            release_proc_slot(child);
            return ret;
        }

        if((ret = copy_stack(parent, child))){
            release_proc_slot(child);
            return ret;
        }

        copy_pregs(parent,child);

        child->time_used = child->sys_time_used = 0;

        child->parent = parent->proc_nr;
        child->thread_parent = 0;
        return child->proc_nr;
    }
    return -EAGAIN;
}

int do_fork(struct proc *who, struct message *m){
    int child_pr;
    struct proc* child;
    child_pr = sys_fork(who);
    
    // if an error is encounted
    if(child_pr < 0)
        return child_pr;

    child = get_proc(child_pr);

    // send 0 to child
    syscall_reply2(FORK, 0, child_pr, m);

    // send the child pid to parent
    syscall_reply2(FORK, child->pid, who->proc_nr, m);
    return DONTREPLY;
}

int do_vfork(struct proc* parent, struct message* m){
    struct proc* child;
    if((child = get_free_proc_slot())){
        copy_pcb(parent,child);
        child->parent = parent->proc_nr;
        child->thread_parent = 0;
        
        syscall_reply2(VFORK, 0, child->proc_nr, m);
        parent->state |= STATE_VFORKING;
        return SUSPEND;
    }
    return -EAGAIN;
}

/**
 * @brief tfork creates a new stack for the child process in the high memory region
 *        it also copies and changes references from old stack to new stack
 * 
 * @param parent 
 * @param m 
 * @return int 
 */
int do_tfork(struct proc* parent, struct message* m){
    struct proc* child;
    int ret;
    
    if((child = get_free_proc_slot())){
        copy_pcb(parent,child);
        child->parent = parent->proc_nr;
        
        if(parent->thread_parent > 0){
            child->thread_parent = parent->thread_parent;
        }else{
            child->thread_parent = parent->proc_nr;
        }
        
        if ((ret = copy_stack(parent, child)))
            return ret;

        /* reply to parent */
        syscall_reply2(TFORK, child->pid, parent->proc_nr, m);

        /* reply to child so child get scheduled first */
        syscall_reply2(TFORK, 0, child->proc_nr, m);
        return DONTREPLY;
    }
    return -EAGAIN;
}


