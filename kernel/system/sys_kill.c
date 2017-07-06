#include "../winix.h"



void syscall_kill(proc_t *who, message_t *m){
    proc_t *to = get_proc(m->i1);

    if(!to){
        m->i1 = -1;
        winix_send(to->pid,m);
        return;
    }

    cause_sig(to,m->i2);
    winix_send(who->pid,m);
}
