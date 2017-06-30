#include "winix.h"
#include <signal.h>

#define SIGFRAME_CODE_LEN   2

static unsigned long sigframe_code[] = {0x1ee10001,0x200d0000};
//addui sp,sp, 1
//syscall

void set_signal(proc_t *caller, int signum, sighandler_t handler){
    caller->sig_table[signum].sa_handler = handler;
}


void real_send_signal(proc_t *who,int signum){
    unsigned long *sp;
    unsigned long *ra;
    static message_t sigret_mesg;

    who->flags = 0;//reset flags
    sp = who->sp;
    sp = get_physical_addr(sp,who);
    
    //restoring the context after sigreturn
    //space for saving the old context
    sp -= PROCESS_CONTEXT_LEN;
    memcpy(sp,who,PROCESS_CONTEXT_LEN );

    //assembly code for invoking sigreturn
    sp -= SIGFRAME_CODE_LEN;
    memcpy(sp,sigframe_code,SIGFRAME_CODE_LEN);
    ra = get_virtual_addr(sp,who);

    //message_t for sigreturn system call
    sigret_mesg.type = SYSCALL_SIGRET;
    sigret_mesg.i1 = signum;
    sp-= MESSAGE_LEN;
    memcpy(sp,&sigret_mesg,MESSAGE_LEN);

    //operating and dest for syscall
    //see exception.c
    sp--;
    *sp = (unsigned long)get_virtual_addr(sp+1,who);
    *--sp = SYSTEM_TASK;
    *--sp = WINIX_SEND;
    

    who->pc = (void (*)())who->sig_table[signum].sa_handler;
    who->ra = ra;
    *--sp = signum;
    who->sp = get_virtual_addr(sp,who);

    // ra = get_physical_addr(who->ra,who);
    // sp = get_physical_addr(who->sp+1,who);
    // kprintf("sig sp %x\n",who->sp);
    // kprintf("sig op %d dest %d mes %x VA ra %x sp %x pc %x\n",*sp, *(sp+1), *(sp+2), who->ra, who->sp,who->sig_table[signum].sa_handler);

    if(current_proc != who){
        current_proc->time_used++;

		if (--current_proc->ticks_left) {
			enqueue_head(ready_q[current_proc->priority], current_proc);
		}
		else { //Re-insert process at the tail of its priority queue
			enqueue_tail(ready_q[current_proc->priority], current_proc);
		}
    }
    delete_proc(ready_q[who->priority],who);
    // process_overview();
    current_proc = who;

    current_proc->ticks_left = current_proc->quantum;

    process_overview();
    printProceInfo(current_proc);
    wramp_load_context();
}

void send_signal(proc_t *who, int signum){
    if(who->sig_table[signum].sa_handler == SIG_DFL){
        kprintf("Taking Default Action: kill %s [%d]",who->name,who->proc_index);
        end_process(who);
        return;
    }
    real_send_signal(who,signum);
}


void do_sigreturn(proc_t *who,int signum){
    unsigned long *sp;

    sp = get_physical_addr(who->sp,who);

    // kprintf("sig ret sp %x\n",sp);

    sp += MESSAGE_LEN + 3 +SIGFRAME_CODE_LEN;
    // kprintf("sig sp %x\n",sp);

    memcpy(who,sp,PROCESS_CONTEXT_LEN);
}
