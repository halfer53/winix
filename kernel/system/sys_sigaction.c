#include "../winix.h"

int set_signal(proc_t *who, int signum, sighandler_t handler){
    who->sig_table[signum].sa_handler = handler;
    return 0;
}

void syscall_sigaction(proc_t *who, message_t *m){
    m->i1 = set_signal(who,m->i1,m->s1);
    winix_send(who->pid,m);
}
