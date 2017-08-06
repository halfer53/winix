#include "../winix.h"

void clear_sending_mesg(struct proc *who){
    register struct proc *rp; //iterate over the process table
    register struct proc **xp; //iterate over the process's queue
    int i;

    for(i = 0; i < NUM_PROCS; i++){
        rp = &proc_table[i];
        if(rp->IN_USE && (xp = &(rp->sender_q)) != NULL){

            //walk through the message queues
            while(*xp && *xp != who)
                xp = &(*xp)->next_sender;
            
            //remove it
            if(*xp)
                *xp = (*xp)->next_sender;
        }
    }
}


void clear_receiving_mesg(struct proc *who){
    register struct proc *xp;
    struct message m;

    if(who->flags & RECEIVING){
        xp = who->sender_q;
        while(xp){
            memset(&m,-1,MESSAGE_LEN);
            winix_notify(xp->proc_nr,&m);
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
    
    unseched(who);
    clear_proc(who);

    // process_overview();
    //if parent is waiting

    for( i=0; i< NUM_PROCS; i++){
        mp = &proc_table[i];
        if(mp->IN_USE){
            if(mp->flags & WAITING && mp->wpid == who->proc_nr){
                //TODO: modify wstatus
            
                mesg.i1 = who->proc_nr;
                mp->flags &= ~WAITING;

                winix_send(mp->proc_nr,&mesg);
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
    who->state = ZOMBIE;
    who->exit_status = status;
    who->IN_USE = 1;
    
}

int do_exit(struct proc *who, struct message *m){
    // kprintf("\r\n[SYSTEM] Process \"%s (%d)\" exited with code %d\r\n", who->name, who->proc_nr, m->i1);
    exit_proc(who,m->i1);
    return SUSPEND;
}



