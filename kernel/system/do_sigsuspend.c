#include <kernel/kernel.h>
#include <winix/sigsend.h>
/**
 * Syscall in this file: sigreturn
 * Input:   m1_i1: set
 *
 * Return:  This syscall does not return
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:10:17
 * 
*/
int do_sigsuspend(struct proc* who, struct message* m){
    sigset_t mask = m->m1_i1;
    sigdelset(&mask, SIGKILL);
    sigdelset(&mask, SIGSTOP);
    who->sig_mask2 = who->sig_mask;
    who->sig_mask = mask;
    if(is_sigpending(who))
        return EINTR;
    
    who->state |= STATE_PAUSING;
    return SUSPEND;
}

