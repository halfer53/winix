#include "../winix.h"

int do_getpid(struct proc *who, struct message *m){
    m->i2 = who->parent;
    return who->proc_nr;
}
