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
