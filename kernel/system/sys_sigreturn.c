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

    resume_syscall(who);
}
