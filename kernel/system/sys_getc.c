#include "../winix.h"

void syscall_getc(proc_t *who, message_t *m){
    int ret;
    m->i1 = kgetc();
    ret = winix_send(who->pid,m);
}
