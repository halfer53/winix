#include <kernel/kernel.h>

int do_sched_yield(struct proc* who, struct message* m){
    who->priority = who->priority == MIN_PRIORITY ? MIN_PRIORITY : who->priority - 1;
    return 0;
}



