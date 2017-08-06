#include "../winix.h"

int do_getpid(struct proc *who, struct message *m){
    return who->proc_nr;
}
