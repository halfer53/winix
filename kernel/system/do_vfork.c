#include <kernel/kernel.h>

int do_vfork(struct proc* parent, struct message* m){
    struct proc* child;
    if(child = get_free_proc_slot()){
        copy_pcb(parent,child);
        copy_pregs(parent, child);
        child->time_used = child->sys_time_used = 0;
        child->parent = parent->proc_nr;
        
        syscall_reply2(VFORK, 0, child->proc_nr, m);
        
        
        parent->state |= STATE_VFORKING;
        return SUSPEND;
    }
    return EAGAIN;
}


