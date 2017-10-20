/**
 * Syscall in this file: kill
 * Input:    m1_i1: pid to which the signal is sent
 *
 * Return:     reply_res:    default return
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:09:49
 * 
*/
#include <kernel/kernel.h>

int do_kill(struct proc *who, struct message *m){
    struct proc *to;
    int ret;
    pid_t pid = m->m1_i1, pgid;
    int signum = m->m1_i2;

    if(signum <= 0 || signum >= _NSIG || pid == 1)
        return EINVAL;

    if(pid > 0){
        to = get_proc_by_pid(pid);
        if(!to)
            return ESRCH;

        return cause_sig(to,signum);
    }

    foreach_proc(to){
        if(pid < -1 && -pid != to->procgrp) continue;
        if(pid == 0 && to->procgrp == who->procgrp) continue;

        cause_sig(to, signum);
    }
    return OK;
}
