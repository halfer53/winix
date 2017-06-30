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


void do_exit(proc_t *caller, message_t *mesg){
    proc_t *parent_mp;
    int parent_pi;

    kprintf("\r\n[SYSTEM] Process \"%s (%d)\" exited with code %d\r\n", caller->name, caller->proc_index, mesg->i1);
    parent_pi = caller->parent_proc_index;
    parent_mp = get_proc(parent_pi);
    
    //if parent is waiting
    if(parent_pi != 0 && parent_mp && parent_mp->flags & WAITING){
        //TODO parse i1 in proper format
        mesg->i1 = caller->proc_index;
        end_process(caller);
        parent_mp->flags &= ~WAITING;
        winix_send(parent_pi,mesg);
    }else{ //if parent is not waiting
        caller->state = ZOMBIE;
        caller->exit_status = mesg->i1;
        //block the current process
    }
}
