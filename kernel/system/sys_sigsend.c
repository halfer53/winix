#include "../winix.h"

static unsigned long sigframe_code[] = {0x1ee10001,0x200d0000};
//addui sp,sp, 1
//syscall

PRIVATE void* build_user_stack(proc_t *who, void *src, size_t len){
    reg_t *sp = get_physical_addr(who->sp,who);
    sp -= len;
    memcpy(sp,src,len );
    return get_virtual_addr(sp,who);
}

PRIVATE void build_signal_ctx(proc_t *who, int signum){
    reg_t *sp;
    reg_t *ra;
    proc_t *systask;
    static message_t sigret_mesg;
    static sigframe_t sigframe;

    sp = who->sp;
    sp = get_physical_addr(sp,who);
    
    who->sp = build_user_stack(who,who,PROCESS_CONTEXT_LEN);
    who->sp = build_user_stack(who,sigframe_code,SIGFRAME_CODE_LEN);

    ra = who->sp;
    sigret_mesg.type = SYSCALL_SIGRET;
    sigret_mesg.i1 = signum;
    who->sp = build_user_stack(who,&sigret_mesg,MESSAGE_LEN);

    sigframe.signum = signum;
    sigframe.operation = WINIX_SEND;
    sigframe.dest = SYSTEM_TASK;
    sigframe.pm = (message_t *)who->sp;
    who->sp = build_user_stack(who,&sigframe,sizeof(sigframe_t));

    who->pc = (void (*)())who->sig_table[signum].sa_handler;
    who->ra = ra;

    who->flags = 0;//reset flags
}

void cause_sig(proc_t *who,int signum){
    if(who->sig_table[signum].sa_handler == SIG_DFL){
        kprintf("Signal %d: kill %s [%d]\n",signum,who->name,who->pid);
        KILL_PROC(who, 1);
        return;
    }
    //if it's ignored
    if(who->sig_table[signum].sa_handler == SIG_IGN){
        kprintf("sig ignored\n");
        who->pc = (void (*)())((int)who->pc+1);
        return;
    }
    build_signal_ctx(who,signum);
}

//IMPORTANT should only be called during exception
void real_send_signal(proc_t *who,int signum){
    
    if(current_proc != who){
        enqueue_tail(ready_q[current_proc->priority], current_proc);
    }
    delete_proc(ready_q[who->priority],who);

    if(who->pid == curr_mesg()->src){
		get_proc(SYSTEM_TASK)->pc = &intr_syscall;
	}

    current_proc = who;
    current_proc->ticks_left = current_proc->quantum;
    
    wramp_load_context();
}

//IMPORTANT should only be called during exception
void send_sig(proc_t *who, int signum){
    cause_sig(who,signum);
    real_send_signal(who,signum);
}

