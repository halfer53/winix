#include "../winix.h"

void clear_sending_mesg(proc_t *who){
    register proc_t *rp; //iterate over the process table
    register proc_t **xp; //iterate over the process's queue
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


void clear_receiving_mesg(proc_t *who){
    register proc_t *xp;
    message_t m;

    if(who->flags & RECEIVING){
        xp = who->sender_q;
        while(xp){
            memset(&m,-1,MESSAGE_LEN);
            winix_notify(xp->pid,&m);
            xp = xp->next_sender;
        }
    }
}

void clear_proc(proc_t *who){
    // clear_receiving_mesg(who);
    clear_sending_mesg(who);
}


void exit_proc(proc_t *who, int status){
    message_t mesg;
    proc_t *mp;
    int i, children = 0;
    
    unseched(who);
    clear_proc(who);

    // process_overview();
    //if parent is waiting

    for( i=0; i< NUM_PROCS; i++){
        mp = &proc_table[i];
        if(mp->IN_USE){
            if(mp->flags & WAITING && mp->wpid == who->pid){
                //TODO: modify wstatus
            
                mesg.i1 = who->pid;
                mp->flags &= ~WAITING;

                winix_send(mp->pid,&mesg);
                children++;
            }else if(mp->parent == who->proc_index){
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

int do_exit(proc_t *who, message_t *m){
    // kprintf("\r\n[SYSTEM] Process \"%s (%d)\" exited with code %d\r\n", who->name, who->pid, m->i1);
	exit_proc(who,m->i1);
    return SUSPEND;
}



