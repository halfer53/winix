/**
 * Syscall in this file: exit
 * Input:   i1: exit status
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

    for(i = 0; i < NUM_PROCS; i++){
        rp = &proc_table[i];
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
            notify(xp->proc_nr,&m);
            xp = xp->next_sender;
        }
    }
}

void clear_proc(struct proc *who){
    // clear_receiving_mesg(who);
    clear_sending_mesg(who);
}


void exit_proc(struct proc *who, int status){
    struct message mesg;
    struct proc *mp;
    int i, children = 0;
    
    unsched(who);
    clear_proc(who);

    // print_runnable_procs();
    //if parent is waiting
    KDEBUG(("%s[%d] exit status %d signal %d\n",who->name, who->proc_nr, 
                                              status, who->sig_status));
    for( i=0; i< NUM_PROCS; i++){
        mp = &proc_table[i];
        if(mp->i_flags & IN_USE){
            if(mp->s_flags & WAITING && mp->wpid == who->proc_nr){
                mesg.i2 = (who->exit_status << 8) | (who->sig_status & 0x7f);
                mp->s_flags &= ~WAITING;

                notify(mp->proc_nr,&mesg);
                children++;
            }else if(mp->parent == who->proc_nr){
                //Change the child process's parent to init
                mp->parent = 1;
            }
        }
    }

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

    exit_proc(who,m->i1);
    return SUSPEND;
}



