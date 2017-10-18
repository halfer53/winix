/**
 * 
 * Signal sending module
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:12:40
 * 
*/
#include <kernel/kernel.h>
#include <kernel/exception.h>
#include <winix/do_signal.h>
/**
    addui $sp, $sp, 1
    syscall
**/

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
 *                 operation is WINIX_SEND, destination is SYSTEM, (or kernel)
 *                 and pm points to the messages being passed to the kernel. NB
 *                 that pm is the virtual memory
 * messages     <- The actual messages, remember that pm points to this message
 * PCB context  <- The previous pcb context saved, will be restored after sigreturn
 */

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
    copyto_user_stack(who,who,SIGNAL_CTX_LEN);
    
    // //ra points at the sigframe code, so that when user signal handler finishes,
    // //pc will point to the sig return code, to initiate sig return sys call
    // who->ra = who->sp - sizeof(struct sigframe_code);
    // sigframe->s_base.operation = WINIX_SENDREC;
    // sigframe->s_base.dest = SYSTEM;
    // //pm points at the syscall message, not that this is a virtual address
    // sigframe->s_base.pm = (struct message *)(who->sp - sizeof(struct sigframe_code) - sizeof(struct message));
    // sigframe->s_base.m.type = SYSCALL_SIGRET;
    // sigframe->s_base.m.m1_i1 = signum;
    // sigframe->s_codes.codes[0] = ASM_ADDUI_SP_SP_1;
    // sigframe->s_codes.codes[1] = ASM_SYSCALL;
    // copyto_user_stack(who,sigframe,sizeof(struct sigframe));

    //signum is sitting on top of the stack
    copyto_user_stack(who, &signum, sizeof(signum));

    who->pc = (void (*)())who->sig_table[signum].sa_handler;
    who->ra = (reg_t*)who->sa_restorer;

    if(who->state){//reschedule the process if it's blocked
        who->state = 0;//reset flags
        enqueue_schedule(who);
    }

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

    if(who->sig_table[signum].sa_handler == SIG_DFL){
        who->sig_status = signum;
        KDEBUG(("Signal %d: kill process \"%s [%d]\"\n",signum,who->name,who->pid));
        
        who->flags |= STOPPED;
        if(in_interrupt()){
            //if we are in interrupt, send an exit syscall to the kernel, 
            //and set current_proc to NULL so the scheduler picks the next proc

            struct message* em = get_exception_m();
            em->type = SYSCALL_EXIT;
            em->m1_i1 = 0;
            em->src = who->proc_nr;
            interrupt_send(SYSTEM, em);
            if(current_proc == who)
                current_proc = NULL;
            
        }else{
            exit_proc(who, 0);
        }
        return OK;
    }
    //if it's ignored
    if(who->sig_table[signum].sa_handler == SIG_IGN){
        KDEBUG(("Signal %d ignored by process \"%s [%d]\"\n",signum,who->name,who->pid));
        who->pc = (void (*)())((int)who->pc+1);
        return OK;
    }
    return ERR;
}

/**
 * send the signal, signal handler will be invoked next time the 
 * process is scheduled
 * @param  who    
 * @param  signum 
 * @return        
 */
int send_sig(struct proc *who, int signum){
    struct sigaction* act;

    if(sys_sig_handler(who,signum) == OK)
        return OK;

    act = &who->sig_table[signum];

    if(act->sa_flags & SA_NODEFER)
        sigaddset(&act->sa_mask, signum);
    else
        sigdelset(&act->sa_mask, signum);

    if(build_signal_ctx(who,signum))
        return EINVAL;
    
    if(who->proc_nr == curr_mesg()->src){
        if(IS_SYSTEM(current_proc)){
            intr_syscall();
        }else{
            get_proc(SYSTEM)->pc = &intr_syscall;
        }
    }

    if(act->sa_flags & SA_RESETHAND)
        act->sa_handler = SIG_DFL;
    return OK;
}

