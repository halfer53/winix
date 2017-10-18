#include <kernel/kernel.h>


int do_sigprocmask(struct proc* who, struct message* m){
    int how = m->m1_i1;
    vptr_t* vset, *oldvset;

    vset = m->m1_p1;
    oldvset = m->m1_p2;

    

    return OK;
}

