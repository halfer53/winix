#include "../winix.h"

int do_getc(proc_t *who, message_t *m){
    // m->i1 = kgetc();
    // ret = winix_send(who->pid,m);
    return kgetc();
}
