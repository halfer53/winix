#include "../winix.h"



int do_kill(proc_t *who, message_t *m){
    proc_t *to = get_proc(m->i1);

    if(!to)
        return EINVAL;
    
    cause_sig(to,m->i2);
    // winix_send(who->pid,m);
    return OK;
}
