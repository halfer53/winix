#include "../winix.h"
#include <sys/wait.h>

void do_wait(proc_t *parent, message_t *mesg){
    register proc_t *child = NULL;
    
    unsigned long *wstatus;
    int children = 0,i;

    for (i=0; i<NUM_PROCS; i++) {
        child = &proc_table[i];
        if(child->IN_USE && child->parent == parent->proc_index){
            if(child->state == ZOMBIE){
                
                //TODO: set wstatus in proper format
                if(mesg->p1 != NULL){
                    wstatus = get_physical_addr(mesg->p1,parent);
                    *wstatus = child->exit_status;
                }
                free_slot(child);
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
        kprintf("no children\n");
        mesg->i1 = -1;
        winix_send(parent->pid,mesg);
        return;
    }
    //block the process
    parent->flags |= WAITING;
}


void syscall_wait(proc_t *who, message_t *m){
    do_wait(who,m);
	m->i1 = 0;
    // winix_send(who->pid,m);
}
