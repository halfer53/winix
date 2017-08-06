#include "../winix.h"

int do_getc(struct proc *who, struct message *m){
    // m->i1 = kgetc();
    // ret = winix_send(who->proc_nr,m);
    return kgetc();
}
