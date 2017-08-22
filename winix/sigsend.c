#include <kernel/kernel.h>
#include <kernel/system.h>
#include <kernel/exception.h>
#include <winix/signal.h>

PRIVATE unsigned int sigframe_code[SIGRET_CODE_LEN] = {0x1ee10001,0x200d0000};
//addui sp,sp, 1
//syscall

int build_user_stack(struct proc *who, void *src, size_t len){
    reg_t *sp = get_physical_addr(who->sp,who);
    sp -= len;
    memcpy(sp,src,len);
    who->sp = get_virtual_addr(sp,who);
    return 0;
}

PRIVATE int build_signal_ctx(struct proc *who, int signum){
    struct sigframe sigframe_s;
    struct sigframe* sigframe = &sigframe_s;

    //pcb context are saved onto the user stack, and will be restored after sigreturn syscall
    build_user_stack(who,who,SIGNAL_CTX_LEN);
    
    //ra points at the sigframe code
    who->ra = who->sp - sizeof(sigframe_code);
    sigframe->operation = WINIX_SEND;
    sigframe->dest = SYSTEM_TASK;
    //pm points at the syscall message
    sigframe->pm = (struct message *)(who->sp - sizeof(sigframe_code) - sizeof(struct message));
    sigframe->m.type = SYSCALL_SIGRET;
    sigframe->m.i1 = signum;
    memcpy(sigframe->sigret_codes, sigframe_code, sizeof(sigframe_code));

    build_user_stack(who,sigframe,sizeof(struct sigframe));
    //signum is sitting on top of the stack
    build_user_stack(who, &signum, sizeof(signum));

    who->pc = (void (*)())who->sig_table[signum].sa_handler;

    if(who->flags)
        enqueue_schedule(who);

    who->flags = 0;//reset flags
    return OK;
}

PRIVATE bool sys_sig_handler(struct proc *who, int signum){
    struct message m;
    
    if(who->sig_table[signum].sa_handler == SIG_DFL){
        int exit_status = SIG_STATUS(signum);
        KDEBUG(("Signal %d: kill %s [%d]\n",signum,who->name,who->proc_nr));
        if(in_interrupt()){
            m.type = SYSCALL_EXIT;
            m.i1 = exit_status;
            m.src = who->proc_nr;
            wini_send(SYSTEM_TASK, &m);
            if(current_proc == who)
                current_proc = NULL;
        }else{
            exit_proc(who, exit_status);
        }
        return OK;
    }
    //if it's ignored
    if(who->sig_table[signum].sa_handler == SIG_IGN){
        KDEBUG(("Signal %d ignored by process \"%s [%d]\"\n",signum,who->name,who->proc_nr));
        who->pc = (void (*)())((int)who->pc+1);
        return OK;
    }
    return ERR;
}

PRIVATE int sigsend_comm(struct proc *who, int signum){
    if(who->state != RUNNABLE)
        return ERR;

    if(build_signal_ctx(who,signum) != OK)
        return ERR;
    
    if(in_interrupt()){
        if(who->proc_nr == curr_mesg()->src){
            get_proc(SYSTEM_TASK)->pc = &intr_syscall;
        }
    }
    return OK;
}

int cause_sig(struct proc *who,int signum){
    if(sys_sig_handler(who,signum) == OK)
        return OK;

    return sigsend_comm(who,signum);
}

//send signal immediately
int send_sig(struct proc *who, int signum){
    if(sys_sig_handler(who,signum) == OK)
        return OK;

    if(sigsend_comm(who,signum) == ERR)
        return ERR;

    if(in_interrupt()){
        if(current_proc != who){
            enqueue_schedule(current_proc);
            dequeue_schedule(who);
        }

        current_proc = who;
        wramp_load_context();
    }
    return OK;
}

