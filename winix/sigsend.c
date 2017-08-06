#include <kernel/kernel.h>
#include <kernel/system.h>
#include <kernel/exception.h>
#include <winix/signal.h>

static unsigned long sigframe_code[] = {0x1ee10001,0x200d0000};
//addui sp,sp, 1
//syscall

PRIVATE void* build_user_stack(struct proc *who, void *src, size_t len){
    reg_t *sp = get_physical_addr(who->sp,who);
    sp -= len;
    memcpy(sp,src,len );
    return get_virtual_addr(sp,who);
}

PRIVATE void build_signal_ctx(struct proc *who, int signum){
    reg_t *sp;
    reg_t *ra;
    struct proc *systask;
    struct message sigret_mesg;
    sigframe_t sigframe;

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
    sigframe.pm = (struct message *)who->sp;
    who->sp = build_user_stack(who,&sigframe,sizeof(sigframe_t));

    who->pc = (void (*)())who->sig_table[signum].sa_handler;
    who->ra = ra;

    who->flags = 0;//reset flags
}

int cause_sig(struct proc *who,int signum){
    if(who->state != RUNNABLE)
        return;
    if(who->sig_table[signum].sa_handler == SIG_DFL){
        kprintf("Signal %d: kill %s [%d]\n",signum,who->name,who->pid);
        KILL_PROC(who, signum);
        return ERR;
    }
    //if it's ignored
    if(who->sig_table[signum].sa_handler == SIG_IGN){
        kprintf("sig %d ignored by %d\n",signum,who->pid);
        who->pc = (void (*)())((int)who->pc+1);
        return ERR;
    }

    build_signal_ctx(who,signum);
    //if in interrupt, let system task interrupt the current syscall
    //and reschedule the proc
    if(in_interrupt()){
        if(who->pid == curr_mesg()->src){
            get_proc(SYSTEM_TASK)->pc = &intr_syscall;
        }
        add_to_scheduling_queue(who);
    }
    return OK;
}

//send signal immediately
//IMPORTANT should only be called during exception
int send_sig(struct proc *who, int signum){
    cause_sig(who,signum);
    if(in_interrupt()){
        if(current_proc != who){
            enqueue_tail(ready_q[current_proc->priority], current_proc);
            remove_from_scheduling_queue(who);
        }

        current_proc = who;
        current_proc->ticks_left = current_proc->quantum;
        
        wramp_load_context();
    }
    return OK;
}

