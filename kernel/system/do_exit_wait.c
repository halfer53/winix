/**
 * Syscall in this file: exit, waitpid
 * 
 * do_exit()
 * Input:   m1_i1: exit status
 * 
 * do_waitpid()
 * Input:   m1_i1: pid
 *          m1_i2: options
 *
 * Return:  reply_res: pid of the child process
 *          m1_i2: wstatus value
 *
 *          if no child is found, this process is blocked
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:09:19
 * 
*/
#include <kernel/kernel.h>
#include <winix/list.h>
#include <sys/wait.h>

int get_wstats(struct proc* child){
    return (child->exit_status << 8) | (child->sig_status & 0x7f);
}

int do_waitpid(struct proc *parent, struct message *mesg){
    struct proc *child;
    struct wait_info* wi;
    pid_t pid;
    int options;
    int children = 0;

    pid = mesg->m1_i1;
    options = mesg->m1_i2;
    
    foreach_child(child, parent){
        /**
         * The value of pid can be:
         * 
         * < -1        meaning wait for any child process whose process group ID is
         *             equal to the absolute value of pid.
         * 
         * -1          meaning wait for any child process.
         * 
         * 0           meaning wait for any child process whose process group ID is
         *             equal to that of the calling process.
         * 
         * > 0         meaning wait for the child whose process ID is equal to the
         *             value of pid.
        **/

        if  (pid > 0 && pid != child->pid) continue;
        if  (pid < -1 && -pid != child->procgrp) continue;
        if  (pid == 0 && child->procgrp != parent->procgrp) continue;

        // if it's a zombie, or a stopped process while WUNTRACED is set
        if( (child->state & STATE_ZOMBIE) ||
            (options & WUNTRACED && child->state & STATE_STOPPED) ){

            mesg->m1_i2 = get_wstats(child);
            release_zombie(child);
            return child->pid;
        }
        children++;
        
    }
    
    if(options & WNOHANG)
        return OK;

    // if this process has no valid children
    if(children <= 0)
        return ECHILD;

    parent->wpid = pid;
    parent->woptions = options;
    parent->state |= STATE_WAITING;
    return SUSPEND;
}

/**
 * Clear all the messages this process is sending
 * @param who 
 */
void clear_sending_mesg(struct proc *who){
    struct proc *rp; // iterate over the process table
    struct proc **xp; // iterate over the process's queue

    foreach_proc_and_task(rp){
        if((xp = &(rp->sender_q)) != NULL && *xp){
            // walk through the message queues
            while(*xp && *xp != who){
                xp = &(*xp)->next_sender;
            }

            // remove it
            if(*xp)
                *xp = (*xp)->next_sender;
        }
    }
}


void clear_proc_mesg(struct proc *who){
    clear_sending_mesg(who);
}


int check_waiting(struct proc* who){
    struct proc* parent = get_proc(who->parent);
    int children = 0;
    struct message* mesg = curr_mesg();

    // if this process if waiting for the current to be exited process
    if(parent && parent->state & STATE_WAITING){
        pid_t pid = parent->wpid;
        if( (pid > 0 && pid == who->pid) ||
            (pid < -1 && -pid == who->procgrp) ||
            (pid == 0 && parent->procgrp == who->procgrp) ||
            (pid == -1)){
            
            // stopped process is only reported if WUNTRACED is set
            if(who->state & STATE_STOPPED && !(parent->woptions & WUNTRACED))
                return ERR;
            
            mesg->type = WAITPID;
            mesg->m1_i2 = get_wstats(who);
            parent->state &= ~STATE_WAITING;
            syscall_reply(who->pid, parent->proc_nr, mesg);
            if(who->state & STATE_ZOMBIE)
                release_zombie(who);
            return OK;
        }
    }
    
    return ERR;
}

void exit_proc_in_interrupt(struct proc* who, int exit_val,int signum){
    struct message* em;
    if(!in_interrupt())
        return;

    // if we are in interrupt, send a EXIT system call
    // to the system on behalf of this process 
    // if most cases, this function is triggered when a signal
    // is sent during exception, refer to kernel/exception.c for
    // more detail e.g. send_sig(current_proc, SIGSEGV)
    em = get_exception_m();
    em->type = EXIT;
    em->m1_i1 = 0;
    em->m1_i2 = signum;
    em->src = who->proc_nr;
    current_proc->state |= STATE_RECEIVING;
    do_send(SYSTEM, em);
    current_proc = NULL;
}

void exit_proc(struct proc *who, int status, int signum){
    struct proc *mp;
    int children = 0;
    struct message* mesg;
    struct proc* parent;

    if(in_interrupt()){
        exit_proc_in_interrupt(who, status, signum);
        return;
    }

    mesg = curr_mesg();
    parent = get_proc(who->parent);

    zombify(who);
    clear_proc_mesg(who);
    who->exit_status = status;
    who->sig_status = signum;

    // When process is created by vfork, parent will be blocked
    // until child _exit(2) or execve(2), to prevent race condition
    // between the parent and child. Since both child and parent share
    // the same memories, so this is special case where we do not release
    // the process memory of the child
    if(parent->state & STATE_VFORKING){
        // parent is blocked by vfork(2)
        parent->state &= ~STATE_VFORKING;
        mesg->type = VFORK;
        syscall_reply(who->pid, parent->proc_nr, mesg);
    }else{
        release_proc_mem(who);
    }

    // child will be adopted by INIT
    foreach_child(mp, who){
        mp->parent = INIT;
    }
    
    // if No process is waiting for this process, send SIGCHLD to parent
    check_waiting(who);
    send_sig(parent, SIGCHLD);
}

int do_exit(struct proc *who, struct message *m){
    int status = m->m1_i1;
    int signum = m->m1_i2;
    struct proc* parent = get_proc(who->parent);
    
    // if exit_magic, this means process is returned
    // from main, and exit syscall is triggered by
    // prologue ( see system/execve.c). The return value
    // in winix is stored in 1
    if(status == EXIT_MAGIC){
        status = who->ctx.m.regs[0];
    }

    KDEBUG(("%s[%d] exit status %d signal %d\n",who->name, who->pid, 
                        status, signum));
    
    exit_proc(who, status, signum);
    
    return SUSPEND;
}


