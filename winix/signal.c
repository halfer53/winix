#include <signal.h>
#include <sys/ipc.h>
#include <sys/syscall.h>
#include <winix/proc.h>
#include <winix/mem_map.h>

#define SIGFRAME_CODE_LEN   2

static unsigned long sigframe_code[] = {0x1ee30001,0x200d0000};

void set_signal(proc_t *caller, int signum, sighandler_t handler){
    caller->sig_table[signum].sa_handler = handler;
}

void send_signal(proc_t *who, int signum){
    unsigned long *sp;
    unsigned long *ra;
    static message_t sigret_mesg;

    sp = who->sp;
    sp = get_physical_addr(sp,who);
    
    //restoring the context after sigreturn
    //space for saving the old context
    sp -= PROCESS_STATE_LEN;
    memcpy(sp,who,PROCESS_STATE_LEN );

    //assembly code for invoking sigreturn
    sp -= SIGFRAME_CODE_LEN;
    memcpy(sp,sigframe_code,SIGFRAME_CODE_LEN);

    sp -= 1;
    *sp = 0x12345678;

    //message_t for sigreturn system call
    sigret_mesg.type = SYSCALL_SIGRET;
    sigret_mesg.i1 = signum;
    sp-= MESSAGE_LEN;
    memcpy(sp,&sigret_mesg,MESSAGE_LEN);

    who->pc = who->sig_table[signum].sa_handler;
    ra = (unsigned long *)who->ra;
    ra = get_virtual_addr(sp + MESSAGE_LEN, who);
    who->ra = ra;
    sp -= 1;
    *sp = signum;
    who->sp = get_virtual_addr(sp,who);

    ra = get_physical_addr(who->ra,who);
    sp = get_physical_addr(who->sp,who);
    kprintf("sig ra %x sp %x\n",*ra,sp);

    current_proc = who;
    wramp_load_context();
}


void do_sigreturn(int signum){
    unsigned long *sp;

    sp = get_physical_addr(current_proc->sp,current_proc);
    sp += MESSAGE_LEN;

    kprintf("sig ret magic %x\n",*sp);

    sp += 1 + SIGFRAME_CODE_LEN;

    memcpy(current_proc,sp,PROCESS_STATE_LEN);
}
