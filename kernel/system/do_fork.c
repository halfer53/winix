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
    INIT_LIST_HEAD(&child->notify_queue);
    INIT_LIST_HEAD(&child->pipe_reading_list);
    INIT_LIST_HEAD(&child->pipe_writing_list);

    for (i = 0; i < OPEN_MAX; ++i) {
        file = child->fp_filp[i];
        if(file){
            file->filp_count += 1;
        }
    }
    return OK;
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
    int j;

    bitmap_clear(child->ctx.ptable, PTABLE_LEN);
    child->mem_start = dup_vm(parent,child);
    if(child->mem_start == NULL)
        return ERR;
    child->ctx.rbase = child->mem_start - child->rbase_offset;
    child->stack_top = child->mem_start;

    // if(parent->flags & DISABLE_FIRST_PAGE){
    //     proc_memctl(child, NULL, PROC_NO_ACCESS);
    // }
    src = (ptr_t *)parent->mem_start;
    dest = (ptr_t *)child->mem_start;
    while(src < parent->heap_bottom){
        copy_page(dest, src);
        src += PAGE_LEN;
        dest += PAGE_LEN;
    }
    return OK;
}


/**
 * copy physical registers. Physical registers are internally used by the kernel
 * Unlike virtual registers like $pc, $ra, some kernel space pointers point to the
 * actually physical memory for fast access
 * @param  parent 
 * @param  child  
 * @return        
 */
int copy_pregs(struct proc* parent, struct proc* child){
    ptr_t *sp;
    sp = get_physical_addr(parent->ctx.m.sp,parent);
    child->message = (struct message *)get_physical_addr(*( sp + 2 ), child);
    child->heap_break = get_physical_addr(get_virtual_addr(parent->heap_break, parent), child);
    child->heap_bottom = get_physical_addr(get_virtual_addr(parent->heap_bottom, parent), child);
    // child->stack_top = get_physical_addr(get_virtual_addr(parent->stack_top, parent), child);
    return OK;
}




/**
 * syscall for fork
 * @param  parent 
 * @return        pid of the child, or -1 if forking is failed
 */
int sys_fork(struct proc *parent) {
    struct proc *child;
    // int tdb_page_len, sp_heap_page_len, page;

    if (child = get_free_proc_slot()) {
        copy_pcb(parent,child);

        if(copy_mm(parent,child)){
            release_proc_slot(child);
            return ENOMEM;
        }

        copy_pregs(parent,child);

        child->time_used = child->sys_time_used = 0;

        child->parent = parent->proc_nr;
        return child->proc_nr;
    }
    return EAGAIN;
}

int do_fork(struct proc *who, struct message *m){
    int child_pr;
    struct proc* child;
    child_pr = sys_fork(who);
    
    
    // if an error is encounted
    if(child_pr < 0)
        return child_pr;
    
    // send 0 to child
    syscall_reply2(FORK, 0, child_pr, m);

    // send the child pid to parent
    return get_proc(child_pr)->pid;
}


