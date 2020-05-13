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
#include <kernel/kernel.h>

int do_sigaction(struct proc *who, struct message *m){
    int signum = m->m3_i1;
    struct sigaction* act = m->m3_p1;
    struct sigaction* oact = m->m3_p2;
    int flags;


    if(!is_vaddr_ok((vptr_t *)act, who))
        return EFAULT;

    if(oact && !is_vaddr_ok((vptr_t *)oact, who))
        return EFAULT;

    if(signum < 1 || signum >= _NSIG)
        return EINVAL;

    if(signum == SIGKILL || signum == SIGSTOP)
        return EINVAL;

    act = (struct sigaction*)get_physical_addr(act, who);
    if(act->sa_handler == SIG_IGN){
        if(signum == SIGSEGV)
            return EINVAL;

        sigdelset(&who->sig_pending, signum);
    }

    if(oact){
        oact = (struct sigaction*)get_physical_addr(oact, who);
        memcpy(oact, &who->sig_table[signum], sizeof(struct sigaction));
    }

    sigdelset(&act->sa_mask, SIGKILL);
    sigdelset(&act->sa_mask, SIGSTOP);
    memcpy(&who->sig_table[signum], act, sizeof(struct sigaction));

    who->sa_restorer = (sighandler_t)(m->m3_f1);
    return OK;
}
