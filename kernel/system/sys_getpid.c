#include "../winix.h"

void syscall_getpid(proc_t *who, message_t *m){
    m->i1 = who->pid;
    winix_send(who->pid,m);
}