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

    if(who->flags & WAITING){
        xp = who->sender_q;
        memset(&m,-1,MESSAGE_LEN);
        while(xp){
            winix_notify(xp->pid,&m);
            xp = xp->next_sender;
        }
    }
}

void clear_proc(proc_t *who){
    release_proc_mem(who);
    clear_receiving_mesg(who);
    clear_sending_mesg(who);
}


void do_exit(proc_t *who, message_t *mesg){
    proc_t *mp;
    int i, children;

    kprintf("\r\n[SYSTEM] Process \"%s (%d)\" exited with code %d\r\n", who->name, who->pid, mesg->i1);
    
    end_process(who);
    clear_proc(who);

    // process_overview();
    //if parent is waiting

    for( i=0; i< NUM_PROCS; i++){
        mp = &proc_table[i];
        if(mp->IN_USE && mp->flags & WAITING && mp->wpid == who->pid){
            //TODO: modify wstatus
            
            mesg->i1 = who->pid;
            mp->flags &= ~WAITING;

            winix_send(mp->pid,mesg);
            children++;
        }
    }

    if(children)
        return;
    
    //if parent is not waiting
    //block the current process
    who->state = ZOMBIE;
    who->exit_status = mesg->i1;
    who->IN_USE = 1;
    
}



