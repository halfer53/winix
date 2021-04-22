#include <kernel/kernel.h>

int do_sched_yield(struct proc* who, struct message* m){
    who->priority = MIN_PRIORITY;
    return OK;
}



