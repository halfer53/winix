/**
 * Syscall in this file: sigaction
 * Input:    
 *
 * Return:     reply_res: syscall status
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:10:09
 * 
*/
#include "../winix.h"

int do_sigaction(struct proc *who, struct message *m){
    int signum;
    sighandler_t osig;

    signum = m->m1_i1;
    if(signum == SIGKILL)
        return EINVAL;
    osig = who->sig_table[signum].sa_handler;
    who->sig_table[signum].sa_handler = m->s1;
    m->s1 = osig;
    return OK;
}
