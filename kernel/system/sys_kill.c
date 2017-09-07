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
#include "../winix.h"

int do_kill(struct proc *who, struct message *m){
    struct proc *to = get_running_proc(m->m1_i1);

    if(!to)
        return ESRCH;

    if(IS_USER_PROC(to)){
        cause_sig(to,m->m1_i2);
        return OK;
    }

    return EINVAL;
}
