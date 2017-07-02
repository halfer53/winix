#include "../winix.h"


void do_sigreturn(proc_t *who,int signum){
    unsigned long *sp;

    sp = get_physical_addr(who->sp,who);

    // kprintf("sig ret sp %x\n",sp);

    sp += MESSAGE_LEN + 3 +SIGFRAME_CODE_LEN;
    kprintf("sigret ");
    printProceInfo(who);
    memcpy(who,sp,PROCESS_CONTEXT_LEN);
}
