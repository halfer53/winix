#include "../winix.h"

int do_vfork(struct proc* parent, struct message* m){
    struct proc* child;
    if(child = get_free_proc_slot()){
        copy_pcb(parent,child);
        
        syscall_reply(0, child->proc_nr, m);
        
        parent->s_flags |= VFORK;
        return SUSPEND;
    }
    return EAGAIN;
}


