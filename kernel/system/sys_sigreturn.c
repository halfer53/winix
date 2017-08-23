/**
 * Syscall in this file: sigreturn
 * Input:   i1: signum number
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
 * @modify date 2017-08-23 06:10:17
*/
#include "../winix.h"

int do_sigreturn(struct proc *who, struct message *m){
    reg_t *sp;
    struct proc *systask;
    int signum = m->i1;

    sp = get_physical_addr(who->sp,who);

    // kprintf("sig ret sp 0x%08x\n",sp);

    sp += sizeof(struct sigframe);
    memcpy(who,sp,SIGNAL_CTX_LEN);

    //reset the signal to default
    who->sig_table[signum].sa_handler = SIG_DFL;
    resume_syscall(who);

    return DONOTHING;
}
