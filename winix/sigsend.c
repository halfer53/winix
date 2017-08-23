#include <kernel/kernel.h>
#include <kernel/system.h>
#include <kernel/exception.h>
#include <winix/signal.h>

PRIVATE unsigned int sigframe_code[SIGRET_CODE_LEN] = {0x1ee10001,0x200d0000};
//addui sp,sp, 1
//syscall

/**
 * How does signal works in winix

 * When signal is to be delivered, it first checks if the signal is set to 
 * default or ignored. If it is, then the kernel choose the default action 
 * according to the signal. Currently, if handler is default, it simply kills the 
 * process. If it's ignored, register $pc moves to the next instruction
 *
 * if neither of those two are set, the signal context will
 * be built on the user stack. The reason we do that is to simulate calling methods
 * for stack trace information.
 *
 * If a user signal action is set (by either signal(2) or sigaction(2)), the kernel
 * first saves process pcb info on to the user stack, which will be later restored 
 * after sig return sys call. Then the sigframe is built onto the stack after this.
 * The sigframe mainly consists code and messages for invoking sigreturn syscall.
 * The structure of the signal context look sth like below
 *
 * signum       <- top of the stack, popped by user signal handler
 * operation   
 * destination
 * pm           <- The above three are necessary parameters for invoking syscall
 *                 operation is WINIX_SEND, destination is SYSTEM_TASK, (or kernel)
 *                 and pm points to the messages being passed to the kernel. NB
 *                 that pm is the virtual memory
 * messages     <- The actual messages, remember that pm points to this message
 * sigret_code  <- assembly code for invoking system call, see sigframe_code,
 *                 This is where ra points to
 * PCB context  <- The previous pcb context saved, will be restored after sigreturn
 */

/**
 * Copy values onto the user stack, this is very similar to memcpy
 * @param  who 
 * @param  src 
 * @param  len 
 * @return     
 */
int build_user_stack(struct proc *who, void *src, size_t len){
    reg_t *sp = get_physical_addr(who->sp,who);
    sp -= len;
    memcpy(sp,src,len);
    who->sp = get_virtual_addr(sp,who);
    return OK;
}

/**
 * Build signal context onto the user stack, PC points to the user
 * signal handler. Once building stack is successful, the next time 
 * the process runs, it will run the next process
 * @param  who    
 * @param  signum 
 * @return        OK if building is successful
 */
PRIVATE int build_signal_ctx(struct proc *who, int signum){
    struct sigframe sigframe_s;
    struct sigframe* sigframe = &sigframe_s;

    //pcb context are saved onto the user stack, and will be restored after sigreturn syscall
    build_user_stack(who,who,SIGNAL_CTX_LEN);
    
    //ra points at the sigframe code, so that when user signal handler finishes,
    //pc will point to the sig return code, to initiate sig return sys call
    who->ra = who->sp - sizeof(sigframe_code);
    sigframe->operation = WINIX_SEND;
    sigframe->dest = SYSTEM_TASK;
    //pm points at the syscall message, not that this is a virtual address
    sigframe->pm = (struct message *)(who->sp - sizeof(sigframe_code) - sizeof(struct message));
    sigframe->m.type = SYSCALL_SIGRET;
    sigframe->m.i1 = signum;
    memcpy(sigframe->sigret_codes, sigframe_code, sizeof(sigframe_code));

    build_user_stack(who,sigframe,sizeof(struct sigframe));
    //signum is sitting on top of the stack
    build_user_stack(who, &signum, sizeof(signum));

    who->pc = (void (*)())who->sig_table[signum].sa_handler;

    if(who->s_flags)
        enqueue_schedule(who);

    who->s_flags = 0;//reset flags
    return OK;
}

/**
 * Check if the system can handle the signal (default, or ignore)
 * @param  who    
 * @param  signum 
 * @return        return OK if system has handled the signal
 *                return ERR if the user needs to handle the signal
 */
PRIVATE int sys_sig_handler(struct proc *who, int signum){
    struct message m;
    
    if(who->sig_table[signum].sa_handler == SIG_DFL){
        int exit_status = SIG_STATUS(signum);
        KDEBUG(("Signal %d: kill process \"%s [%d]\"\n",signum,who->name,who->proc_nr));
        if(in_interrupt()){
            //if we are in interrupt, send an exit syscall to the kernel, 
            //and set current_proc to NULL so the scheduler picks the next proc
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

/**
 * commond signal send function for both cause_sig and send_sig
 * @param  who    
 * @param  signum 
 * @return        
 */
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

/**
 * cause the signal, signal handler will be invoked next time the 
 * process is scheduled
 * @param  who    
 * @param  signum 
 * @return        
 */
int cause_sig(struct proc *who,int signum){
    if(sys_sig_handler(who,signum) == OK)
        return OK;

    return sigsend_comm(who,signum);
}

/**
 * In contrast to cause_sig, send_sig is usually called during exception
 * it delivers the signal immediately
 * @param  who    
 * @param  signum 
 * @return        
 */
int send_sig(struct proc *who, int signum){
    if(sys_sig_handler(who,signum) == OK)
        return OK;

    if(sigsend_comm(who,signum) == ERR)
        return ERR;

    if(in_interrupt()){
        //send_sig is usually called during exception
        //so we manually add the current proc to the ready queue
        //and schedule the process imeediately
        if(current_proc != who){
            enqueue_schedule(current_proc);
            dequeue_schedule(who);
        }

        current_proc = who;
        wramp_load_context();
    }
    return OK;
}

