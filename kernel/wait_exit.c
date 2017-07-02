#include "winix.h"
#include <sys/wait.h>

void do_wait(proc_t *parent, message_t *mesg){
    register proc_t *child = NULL;
    
    unsigned long *wstatus;
    int children,i;

    for (i=0; i<NUM_PROCS; i++) {
        child = &proc_table[i];
        if(child->parent_proc_index == parent->proc_index){
            if(child->state == ZOMBIE){
                kprintf("find zomebie child immed\n");
                //TODO: set wstatus in proper format
                if(mesg->p1 != NULL){
                    wstatus = get_physical_addr(mesg->p1,parent);
                    *wstatus = child->exit_status;
                }
                end_process(child);
                mesg->i1 = child->proc_index;
                winix_send(parent->proc_index,mesg);
                return;
            }
            children++;
        }
    }
	

    if(children == 0){
        //TODO: set error status on wstatus
        kprintf("This proc has no children\n");
        winix_send(parent->proc_index,mesg);
        return;
    }

    //block the process
    parent->flags |= WAITING;
}


void clear_proc(proc_t *caller){
    register proc_t *rp; //iterate over the process table
    register proc_t *xp; //iterate over the process's queue
    register proc_t *prev_xp;
    int i;
    /**TODO: release memory
            release messages that curr proc is waiting for
            release messages from which other procs are waiting for
    **/


    for(i = 0; i < NUM_PROCS; i++){
        rp = &proc_table[i];
        if(rp->IN_USE && rp->sender_q != NULL){
            xp = rp->sender_q;
            do{
                if(xp == caller){

                }
            }while(xp != NULL);
        }
    }
}


void do_exit(proc_t *caller, message_t *mesg){
    proc_t *parent_mp;
    int parent_pi;

    kprintf("\r\n[SYSTEM] Process \"%s (%d)\" exited with code %d\r\n", caller->name, caller->proc_index, mesg->i1);
    parent_pi = caller->parent_proc_index;
    parent_mp = get_proc(parent_pi);

    kprintf("do_exit ");
    printProceInfo(caller);
    printProceInfo(get_proc(2));

    end_process(caller);
    //if parent is waiting
    if(parent_mp && parent_mp->flags & WAITING){
        //TODO: parse i1 in proper format
        
        mesg->i1 = caller->proc_index;
        parent_mp->flags &= ~WAITING;

        winix_send(parent_pi,mesg);
    }else{ //if parent is not waiting
        caller->state = ZOMBIE;
        caller->exit_status = mesg->i1;
        //block the current process
    }
    

    DEBUG = 10;
}
