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
#include "../winix.h"

/**
 * Clear all the messages this process is sending
 * @param who 
 */
void clear_sending_mesg(struct proc *who){
    register struct proc *rp; //iterate over the process table
    register struct proc **xp; //iterate over the process's queue
    int i;

    for_each_user_proc(rp){
        if(rp->i_flags & IN_USE && (xp = &(rp->sender_q)) != NULL){

            //walk through the message queues
            while(*xp && *xp != who)
                xp = &(*xp)->next_sender;
            
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

    if(who->s_flags & RECEIVING){
        xp = who->sender_q;
        while(xp){
            memset(&m,-1,sizeof( struct message));
            syscall_reply(-1, xp->proc_nr,&m);
            xp = xp->next_sender;
        }
    }
}

void clear_proc_mesg(struct proc *who){
    // clear_receiving_mesg(who);
    clear_sending_mesg(who);
}


void exit_proc(struct proc *who, int status){
    struct proc *mp;
    int i, children = 0;
    struct message* mesg = curr_mesg();

    KPRINT_DEBUG(("%s[%d] exit status %d signal %d\n",who->name, who->proc_nr, 
                                              status, who->sig_status));

    unsched(who);
    clear_proc_mesg(who);
    who->exit_status = status;      

    for_each_user_proc(mp){
        if(mp->i_flags & IN_USE){
            //if this process if waiting for the current tobe exited process
            if(mp->s_flags & WAITING && mp->wpid == who->proc_nr){

                mesg->m1_i2 = (who->exit_status << 8) | (who->sig_status & 0x7f);
                mp->s_flags &= ~WAITING;
                syscall_reply(who->proc_nr, mp->proc_nr, mesg);

                children++;
            }else if(mp->parent == who->proc_nr){
                //Change the child process's parent to init
                mp->parent = 1;
            }else if(mp->s_flags & VFORK){  //parent is blocked by vfork(2)

                mp->s_flags &= ~VFORK;
                
                syscall_reply(who->proc_nr, mp->proc_nr, mesg);
                return;
            }
        }
    }

    release_proc_mem(who);

    if(children){
        free_slot(who);
        return;
    }
        
    //if parent is not waiting
    //block the current process
    who->exit_status = status;
    who->i_flags |= IN_USE;
}

int do_exit(struct proc *who, struct message *m){
    int status = m->m1_i1;
    if(status == EXIT_MAGIC){
        status = who->regs[0];
    }
    exit_proc(who,status);
    return SUSPEND;
}



