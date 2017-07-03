#include "../winix.h"
#include <sys/wait.h>

void do_wait(proc_t *parent, message_t *mesg){
    register proc_t *child = NULL;
    
    unsigned long *wstatus;
    int children,i;

    for (i=0; i<NUM_PROCS; i++) {
        child = &proc_table[i];
        if(child->IN_USE && child->parent == parent->proc_index){
            if(child->state == ZOMBIE){
                //TODO: set wstatus in proper format
                if(mesg->p1 != NULL){
                    wstatus = get_physical_addr(mesg->p1,parent);
                    *wstatus = child->exit_status;
                }
                end_process(child);
                mesg->i1 = child->pid;
                winix_send(parent->pid,mesg);
                return;
            }
            parent->wpid = child->pid;
            children++;
        }
    }
	
    //Proc has no children
    if(children == 0){
        mesg->i1 = -1;
        winix_send(parent->pid,mesg);
        return;
    }
    //block the process
    parent->flags |= WAITING;
}
