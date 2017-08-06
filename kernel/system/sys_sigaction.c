#include "../winix.h"

int do_sigaction(struct proc *who, struct message *m){
    int signum;
    sighandler_t osig;

    signum = m->i1;
    if(signum == SIGKILL)
        return EINVAL;
    osig = who->sig_table[signum].sa_handler;
    who->sig_table[signum].sa_handler = m->s1;
    m->s1 = osig;
    return OK;
}
