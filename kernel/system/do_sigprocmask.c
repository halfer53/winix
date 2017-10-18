#include <kernel/kernel.h>
#include <winix/sigsend.h>
/**
 * Syscall in this file: sigprocmask
 * Input:   m1_i1: how
 *          m1_i2: set
 *
 * Return:  m1_i2: old set;
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

int do_sigprocmask(struct proc* who, struct message* m){
    sigset_t set;
    sigset_t *pset, *pblocked;
    int i;
    int how = m->m1_i1;

    m->m1_i1 = who->sig_mask;
    
    set = m->m1_i2;
    pset = &set;
    pblocked = &who->sig_mask;
    sigdelset(pset, SIGKILL);
    sigdelset(pset, SIGSTOP);

    switch(how){
        case SIG_SETMASK:
            who->sig_mask = set;
            break;

        case SIG_BLOCK:
            for( i = 1; i < _NSIG; i++){
                if(sigismember(pset, i)){
                    sigaddset(pblocked, i);
                }
            }
            break;

        case SIG_UNBLOCK:
            for( i = 1; i < _NSIG; i++){
                if(sigismember(pset, i)){
                    sigdelset(pblocked, i);
                }
            }
            break;

        default:
            return EINVAL;
    }

    check_sigpending(who);
    return OK;
}

