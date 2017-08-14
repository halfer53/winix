#include "../winix.h"



int do_kill(struct proc *who, struct message *m){
    struct proc *to = get_running_proc(m->i1);

    if(!to)
        return EINVAL;
    
    cause_sig(to,m->i2);
    // winix_send(who->proc_nr,m);
    return OK;
}
