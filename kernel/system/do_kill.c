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
    pid_t pid = m->m1_i1;
    int signum = m->m1_i2;

    if(signum <= 0 || signum >= _NSIG || pid == 1)
        return EINVAL;

    to = get_proc_by_pid(pid);
    if(!to)
        return ESRCH;

    ret = sig_proc(to,signum);

    //Don't reply if the process sends a signal to itself
    // if(to == who)
    //     return DONTREPLY;
    return ret;
}
