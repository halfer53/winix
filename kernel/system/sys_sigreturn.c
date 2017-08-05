#include "../winix.h"


int do_sigreturn(proc_t *who, message_t *m){
    reg_t *sp;
    proc_t *systask;
    int signum = m->i1;

    sp = get_physical_addr(who->sp,who);

    // kprintf("sig ret sp 0x%08x\n",sp);

    sp += MESSAGE_LEN + 3 +SIGFRAME_CODE_LEN;
    memcpy(who,sp,PROCESS_CONTEXT_LEN);

    //reset the signal to default
    who->sig_table[signum].sa_handler = SIG_DFL;
    resume_syscall(who);

    return DONOTHING;
}