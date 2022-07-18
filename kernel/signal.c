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
#include <winix/ksignal.h>
#include <winix/kdebug.h>

/**
 * How does signal works in winix

 * When signal is to be delivered, it first checks if the signal is set to 
 * default or ignored. If it is, then the kernel choose the default action 
 * according to the signal. Currently, if handler is default, it simply kills the 
 * process. If it's ignored, $pc moves to the next instruction
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
 * signum           <- top of the stack, used by user signal handler
 * syscall_num      <- 
 * ASM: pop stack   <- ra points to this, it will pop the stack, thus pointing stack
 *                      to syscall_num, then invoke syscall
 * ASM: syscall
 * PCB context  <- The previous pcb context saved, will be restored after sigreturn
 */

/**
 * Build signal context onto the user stack, PC points to the user
 * signal handler. Once building stack is successful, the next time 
 * the process runs, it will run the next process
 * @param  who    
 * @param  signum 
 * @return        0 if building is successful
 */
PRIVATE int build_signal_ctx(struct proc *who, int signum){
    struct sigframe sframe;
    // pcb context are saved onto the user stack, and will be restored after sigreturn syscall

    copyto_user_stack(who, who, SIGNAL_CTX_LEN);

    sframe.syscall_num = SIGRET;
    sframe.code = ASM_ADDUI_SP_SP_1;
    sframe.code2 = ASM_SYSCALL;
    copyto_user_stack(who, &sframe, sizeof(struct sigframe));
    who->ctx.m.ra = who->ctx.m.sp + 1;

    // signum is sitting on top of the stack
    copyto_user_stack(who, &signum, sizeof(signum));
    
    // backup sig mask
    who->sig_mask2 = who->sig_mask;
    who->sig_mask = who->sig_table[signum].sa_mask;

    who->ctx.m.pc = (void (*)())who->sig_table[signum].sa_handler;
    who->state = STATE_RUNNABLE;
    who->flags |= PROC_SIGAL_HANDLER;
    return 0;
}

/**
 * Check if the system can handle the signal (default, or ignore)
 * @param  who    
 * @param  signum 
 * @return        return 0 if system has handled the signal
 *                return -EINVAL if the user needs to handle the signal
 */
PRIVATE int sys_sig_handler(struct proc *who, int signum){
    sighandler_t handler = who->sig_table[signum].sa_handler;
    if(handler == SIG_DFL){

        switch(signum){
            case SIGCONT:
                if(who->state & STATE_STOPPED){
                    who->state &= ~STATE_STOPPED;
                    if (who->state & STATE_RECEIVING){
                        who->state &= ~STATE_RECEIVING;
                        (void)set_syscall_reply(who, -EINTR, 0);
                    }
                    if(who->state == STATE_RUNNABLE){
                        enqueue_schedule(who);
                    }
                }
                break;

            case SIGTSTP:
            case SIGSTOP:
                who->state |= STATE_STOPPED;
                who->sig_status = signum;
                dequeue_schedule(who);
                check_waiting(who);
                break;
            
            default:
                klog("Signal %d: terminate process \"%s [%d]\"\n"
                                        ,signum,who->name,who->pid);
                // KDEBUG(("Signal curr %x\n", curr_scheduling_proc));
                exit_proc(who, 128, signum);
        }
    }
    // if it's ignored
    else if(handler == SIG_IGN){
        struct proc* mp;
        switch(signum){
            case SIGABRT:
                exit_proc(who, 0, signum);
                break;
            
            // case SIGSEGV:
            //     who->sig_table[SIGSEGV].sa_handler = SIG_DFL;
            //     break;
                
            case SIGCHLD:
                foreach_child(mp, who){
                    if(mp->state & STATE_ZOMBIE){
                        release_zombie(mp);
                        break;
                    }
                }
                break;

            default:
                KDEBUG(("Signal %d ignored by process \"%s [%d]\"\n"
                                ,signum,who->name,who->pid));
                break;
        }
    }
    return 0;
}

/**
 * don't invoke this unless the signal is checked by
 * is_sigpending()
 */
int handle_sig(struct proc* who, int signum){
    struct sigaction* act;
    sighandler_t handler = who->sig_table[signum].sa_handler;
    // KDEBUG(("handle %d for %d\n", signum, who->proc_nr));
    sigdelset(&who->sig_pending, signum);
    // if the system can handle the signal
    if (handler == SIG_DFL || handler == SIG_IGN)
        return sys_sig_handler(who,signum);

    act = &who->sig_table[signum];

    if(act->sa_flags & SA_NODEFER)
        sigaddset(&act->sa_mask, signum);
    else
        sigdelset(&act->sa_mask, signum);

    build_signal_ctx(who, signum);

    if(act->sa_flags & SA_RESETHAND){
        act->sa_handler = SIG_DFL;
    }

    return 0;
}

int send_sig(struct proc *who, int signum){

    if(!IS_USER_PROC(who))
        return -EINVAL;
    
    // if this signal is blocked
    if(sigismember(&who->sig_mask, signum)){
        if(signum != SIGKILL && signum != SIGSTOP){
            int ret = 0;
            
            // if a signal is ignored and blocked by the process
            // it is quietly ignored, and not pended
            if(who->sig_table[signum].sa_handler != SIG_IGN){
                ret = sigaddset(&who->sig_pending, signum);
            }
            return ret;
        }
    }

    // Unpause the process if it was blocked by pause(2)
    // or sigsuspend(2)
    if(who->state & STATE_PAUSING ){
        struct message m;
        m.type = 0;
        who->state &= ~STATE_PAUSING;
        syscall_reply2(0, -EINTR, who->proc_nr, &m);
    }

    // add it the list of pending signals
    // whenever the scheduler is called, it checks if the
    // process to be sched have any pending signals. This is
    // when handle_sig will be called.
    sigaddset(&who->sig_pending, signum);
    
    
    return 0;
}



int is_sigpending(struct proc* who){
    int i, sigmask;
    sigset_t pendings = who->sig_pending;
    sigset_t blocked = who->sig_mask;

    if(pendings && pendings != blocked){
        for(i = 1; i < _NSIG; i++){
            sigmask = 1 << i;
            if(pendings & sigmask && !(blocked & sigmask)){
                return i;
            }
        }
    }
    return 0;
}

int handle_pendingsig(struct proc* who, bool check_enqueue){
    int signum = is_sigpending(who);
    if(signum){
        handle_sig(who, signum);
        // (syscall_return == DONTREPLY || syscall_return == SUSPEND)
        if(check_enqueue && who->state == STATE_RUNNABLE && who->flags & PROC_SIGAL_HANDLER){
            enqueue_schedule(who);
        }
    }
    return signum;
}
