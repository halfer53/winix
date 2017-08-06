#include "../winix.h"

int do_getppid(struct proc *who, struct message *m){
    // m->i1 = get_proc(who->parent)->proc_nr;
    return get_proc(who->parent)->proc_nr;
}
