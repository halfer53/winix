#include "../winix.h"


void do_sigreturn(proc_t *who,int signum){
    unsigned long *sp;
    proc_t *systask;

    sp = get_physical_addr(who->sp,who);

    // kprintf("sig ret sp %x\n",sp);

    sp += MESSAGE_LEN + 3 +SIGFRAME_CODE_LEN;
    memcpy(who,sp,PROCESS_CONTEXT_LEN);

    if(who->flags){
        delete_proc(ready_q[who->priority],who);
    }

    //reset the signal to default
    who->sig_table[signum].sa_handler = SIG_DFL;
    resume_syscall(who);
}


void syscall_sigreturn(proc_t *who, message_t *m){
    do_sigreturn(who,m->i1);
    m->i1 = 0;
    // winix_send(who->pid,m);
}
