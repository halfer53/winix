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

int sys_sigaction(struct proc* who, int signum, struct sigaction* act, struct sigaction* oact, void *restorer){
    if(signum < 1 || signum >= _NSIG)
        return EINVAL;

    if(signum == SIGKILL || signum == SIGSTOP)
        return EINVAL;

    if(act->sa_handler == SIG_IGN){
        if(signum == SIGSEGV)
            return EINVAL;

        sigdelset(&who->sig_pending, signum);
    }

    if(oact){
        memcpy(oact, &who->sig_table[signum], sizeof(struct sigaction));
    }

    sigdelset(&act->sa_mask, SIGKILL);
    sigdelset(&act->sa_mask, SIGSTOP);
    memcpy(&who->sig_table[signum], act, sizeof(struct sigaction));

    who->sa_restorer = (reg_t*)restorer;
    return OK;
}

int do_sigaction(struct proc *who, struct message *m){
    int signum = m->m1_i1;
    struct sigaction* act = m->m1_p1;
    struct sigaction* oact = m->m1_p2;
    int flags;


    if(!is_vaddr_ok((vptr_t *)act, who))
        return EFAULT;

    if(oact && !is_vaddr_ok((vptr_t *)oact, who))
        return EFAULT;

    if(!is_vaddr_accessible(m->m1_p3, who))
        return EFAULT;

    act = (struct sigaction*)get_physical_addr(act, who);

    if(oact){
        oact = (struct sigaction*)get_physical_addr(oact, who);
    }
    
    return sys_sigaction(who, signum, act, oact, m->m1_p3);
}


int do_signal(struct proc* who, struct message *m){
    struct sigaction sa, oldsa;
    int signum = m->m1_i1;
    void *restorer = m->m1_p1;
    
    sa.sa_handler = (sighandler_t)m->m1_i2;
    sa.sa_flags = SA_RESETHAND;
    sa.sa_mask = 0xffff;
    if(sys_sigaction(who, signum, &sa, &oldsa, restorer))
        return (int)SIG_ERR;
    return (int)oldsa.sa_handler;
}

