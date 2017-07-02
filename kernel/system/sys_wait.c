#include "../winix.h"
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
