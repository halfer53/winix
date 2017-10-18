#include <kernel/kernel.h>

int do_setpgid(struct proc* who, struct message* m){
    pid_t pid = m->m1_i1;
    pid_t pgid = m->m1_i2;
    struct proc* mp;


    if(pid< 0 || pgid < 0)
        return EINVAL;

    if(pid == 0)
        mp = who;
    else
        mp = get_proc_by_pid(pid);

    if(!mp)
        return ESRCH;

    if(pgid == 0)
        pgid = mp->pid;

    mp->procgrp = pgid;
    return OK;
}

