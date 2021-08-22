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
    child->notify_pending = 0;
    INIT_LIST_HEAD(&child->pipe_reading_list);
    INIT_LIST_HEAD(&child->pipe_writing_list);

    for (i = 0; i < OPEN_MAX; ++i) {
        file = child->fp_filp[i];
        if(file){
            file->filp_count += 1;
        }
    }

    if(parent->priority > MIN_PRIORITY) // let child run first
        parent->priority--;
    parent->ticks_left = 0;
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
    child->stack_top = get_physical_addr(get_virtual_addr(parent->stack_top, parent), child);
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
        child->thread_parent = 0;

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

int do_vfork(struct proc* parent, struct message* m){
    struct proc* child;
    if(child = get_free_proc_slot()){
        copy_pcb(parent,child);
        child->time_used = child->sys_time_used = 0;
        child->parent = parent->proc_nr;
        child->thread_parent = 0;
        
        syscall_reply2(VFORK, 0, child->proc_nr, m);
        parent->state |= STATE_VFORKING;
        return SUSPEND;
    }
    return EAGAIN;
}

int do_tfork(struct proc* parent, struct message* m){
    struct proc* child;
    ptr_t* new_stack, *sp_physical, *old_stack;
    unsigned int *stack_ptr, *stack_bottom;
    unsigned int vstack_top, vstack_bottom, val;
    vptr_t* vsp_relative_to_stack_top, *vir_old_stack;
    reg_t** sp;
    if(child = get_free_proc_slot()){
        copy_pcb(parent,child);
        child->time_used = child->sys_time_used = 0;
        child->parent = parent->proc_nr;
        if(parent->thread_parent > 0){
            child->thread_parent = parent->thread_parent;
        }else{
            child->thread_parent = parent->proc_nr;
        }
        
        new_stack = user_get_free_page(child, GFP_HIGH);
        if(new_stack == NULL){
            release_proc_slot(child);
            return ENOMEM;
        }
        copy_page(new_stack, child->stack_top);
        sp = &child->ctx.m.sp;
        vsp_relative_to_stack_top = (vptr_t*)(get_physical_addr(*sp, child) - child->stack_top);
        sp_physical = (ptr_t *)(new_stack + (ptr_t)vsp_relative_to_stack_top) ;
        *sp = (reg_t *)get_virtual_addr(sp_physical, child);
        old_stack = child->stack_top;
        child->stack_top = new_stack;

        stack_ptr = (unsigned int*)child->stack_top;
        stack_bottom = (unsigned int*)stack_ptr + PAGE_LEN;
        vir_old_stack = get_virtual_addr(old_stack, child);
        vstack_top = (unsigned int)vir_old_stack;
        vstack_bottom = (unsigned int)vir_old_stack + PAGE_LEN;

        // change the virtual address referencing old stack to new stack
        while (stack_ptr < stack_bottom) {
            val = *stack_ptr;
            if (val >= vstack_top && val < vstack_bottom) {
                *stack_ptr = (unsigned int)get_virtual_addr(val - vstack_top + new_stack, child);
                // KDEBUG(("old %x new %x\n", val, *stack_ptr));
            }
            stack_ptr++;
        }

        // KDEBUG(("tfork %x %x for %d tp %d\n", new_stack, *sp, child->proc_nr, child->thread_parent));
        proc_memctl(child, vir_old_stack, PROC_NO_ACCESS);
        syscall_reply2(TFORK, 0, child->proc_nr, m);
        return child->proc_nr;
    }
    return EAGAIN;
}


