#include <kernel/kernel.h>

int do_setsid(struct proc* who, struct message* m){
    pid_t pid = who->pid;
    if (who->procgrp == pid){
        return -EPERM;
    }
    who->session_id = pid;
    who->procgrp = pid;
    return pid;
}

