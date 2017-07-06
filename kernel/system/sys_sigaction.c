#include "../winix.h"

int do_sigaction(proc_t *who, message_t *m){
    int signum = m->i1;
    sighandler_t osig = who->sig_table[signum].sa_handler;
    who->sig_table[signum].sa_handler = m->s1;
    m->s1 = osig;
    return OK;
}
