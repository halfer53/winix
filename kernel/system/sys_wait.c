#include "../winix.h"
#include <sys/wait.h>

int do_wait(struct proc *parent, struct message *mesg){
    register struct proc *child = NULL;
    
    ptr_t* wstatus;
    int children = 0,i;

    for (i=0; i<NUM_PROCS; i++) {
        child = &proc_table[i];
        if(child->IN_USE && child->parent == parent->proc_nr){
            if(child->state == ZOMBIE){
                
                //TODO: set wstatus in proper format
                if(mesg->p1 != NULL){
                    wstatus = get_physical_addr(mesg->p1,parent);
                    *wstatus = child->exit_status;
                }
                free_slot(child);
                kprintf("%d child %d\n",child->proc_nr);
                mesg->i1 = child->proc_nr;
                winix_send(parent->proc_nr,mesg);
                return OK;
            }
            parent->wpid = child->proc_nr;
            children++;
        }
    }
	
    //Proc has no children
    if(children == 0){
        kprintf("no children\n");
        mesg->i1 = -1;
        // winix_send(parent->proc_nr,mesg);
        return ECHILD;
    }
    //block the process
    parent->flags |= WAITING;
    return SUSPEND;
}
