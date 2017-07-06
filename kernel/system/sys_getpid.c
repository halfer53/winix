#include "../winix.h"

int do_getpid(proc_t *who, message_t *m){
    m->i1 = who->pid;
    // winix_send(who->pid,m);
    return OK;
}
