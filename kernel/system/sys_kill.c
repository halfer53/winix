#include "../winix.h"



int do_kill(struct proc *who, struct message *m){
    struct proc *to = get_running_proc(m->i1);

    if(!to)
        return ESRCH;

    if(to->proc_nr <= 1)
        return EINVAL;
    
    cause_sig(to,m->i2);
    return OK;
}
