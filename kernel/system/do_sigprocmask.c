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
    sigset_t set, set_bak;
    sigset_t *pset, *pblocked;
    int i;
    int how = m->m1_i1;
    vptr_t *vir_oldact;
    ptr_t *oldact;

    vir_oldact = m->m1_p1;

    if(vir_oldact && !is_vaddr_accessible(vir_oldact, who))
        return EFAULT;
    if(vir_oldact){
        oldact = get_physical_addr(vir_oldact, who);
        *oldact = who->sig_mask;
    }
    
    set = m->m1_i2;
    pset = &set;
    pblocked = &who->sig_mask;
    sigdelset(pset, SIGKILL);
    sigdelset(pset, SIGSTOP);

    switch(how){
        case SIG_SETMASK:
            who->sig_mask = set;
            // KDEBUG(("set sigmask %x for %d\n", set, who->pid));
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

    return OK;
}

