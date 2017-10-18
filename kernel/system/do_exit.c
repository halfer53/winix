/**
 * Syscall in this file: exit
 * Input:   m1_i1: exit status
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


/**
 * Clear all the messages this process is sending
 * @param who 
 */
void clear_sending_mesg(struct proc *who){
    register struct proc *rp; //iterate over the process table
    register struct proc **xp; //iterate over the process's queue

    foreach_proc_and_task(rp){
        if((xp = &(rp->sender_q)) != NULL && *xp){
            //walk through the message queues
            while(*xp && *xp != who){
                xp = &(*xp)->next_sender;
            }

            //remove it
            if(*xp)
                *xp = (*xp)->next_sender;
        }
    }
}

/**
 * return error to all the process who are waiting for messages 
 * for the current process
 * This is not triggered since this is really a user space problem
 * But we just keep it here for future use
 * @param who 
 */
void clear_receiving_mesg(struct proc *who){
    register struct proc *xp;
    struct message m;

    if(who->state & RECEIVING){
        xp = who->sender_q;
        while(xp){
            memset(&m,-1,sizeof( struct message));
            syscall_reply(-1, xp->proc_nr,&m);
            xp = xp->next_sender;
        }
    }
}

void clear_proc_mesg(struct proc *who){
    clear_sending_mesg(who);
}


void exit_proc(struct proc *who, int status){
    struct proc *mp;
    int children = 0;
    struct message* mesg = curr_mesg();
    struct proc* parent = get_proc(who->parent);

    KDEBUG(("%s[%d] exit status %d signal %d\n",who->name, who->pid, 
                                              status, who->sig_status));

    zombify(who);
    clear_proc_mesg(who);
    who->exit_status = status;
    who->flags |= STOPPED;

    if(parent->state & VFORKING){
        //parent is blocked by vfork(2)
        parent->state &= ~VFORKING;
        syscall_reply(who->pid, parent->proc_nr, mesg);
        release_zombie(who);
        return;
    }
    send_sig(parent, SIGCHLD);

    foreach_proc(mp){
        //if this process if waiting for the current to be exited process
        if(mp->state & WAITING && mp->wpid == who->pid){
            mesg->m1_i2 = get_wstats(who);
            mp->state &= ~WAITING;
            syscall_reply(who->pid, mp->proc_nr, mesg);
            children++;

        }else if(mp->parent == who->proc_nr){
            //Change the child process's parent to init
            mp->parent = INIT;
        }
    }

    release_proc_mem(who);

    if(children){
        release_zombie(who);
        return;
    }

    //if parent is not waiting
    //block the current process
    who->flags |= IN_USE;
}

int do_exit(struct proc *who, struct message *m){
    int status = m->m1_i1;
    
    //if exit_magic, this means process is returned
    //from main, and exit syscall is triggered by
    //prologue ( see system/execve.c). The return value
    //in winix is stored in register 1
    if(status == EXIT_MAGIC){
        status = who->regs[0];
    }
    exit_proc(who,status);
    return SUSPEND;
}



