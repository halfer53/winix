#include "../winix.h"

int do_sigaction(proc_t *who, message_t *m){
    int signum;
    sighandler_t osig;

    DEBUG_IPC = 20;
    DEBUG_SCHED = 30;
    signum = m->i1;
    if(signum == SIGKILL)
        return EINVAL;
    osig = who->sig_table[signum].sa_handler;
    who->sig_table[signum].sa_handler = m->s1;
    m->s1 = osig;
    return OK;
}
