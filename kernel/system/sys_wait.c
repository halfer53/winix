#include "../winix.h"
#include <sys/wait.h>

/**
 * Syscall in this file: wait
 * Input:   
 *
 * Return:  i1: pid of the child process
 *          i2: wstatus value
 *
 *          if no child is found, this process is blocked
 * 
 */
int do_wait(struct proc *parent, struct message *mesg){
    register struct proc *child = NULL;
    
    ptr_t* wstatus;
    int children = 0,i;

    for (i=0; i<NUM_PROCS; i++) {
        child = &proc_table[i];
        
        //if there is a child process that is zombie
        if(child->i_flags & PROC_IN_USE && child->parent == parent->proc_nr){
            if(child->state == ZOMBIE){
                free_slot(child);
                mesg->i2 = (child->exit_status << 8) | (child->sig_status & 0377);
                return child->proc_nr;
            }
            parent->wpid = child->proc_nr;
            children++;
        }
    }
	
    //Proc has no children
    if(children == 0){
        mesg->i1 = -1;
        // winix_send(parent->proc_nr,mesg);
        return ECHILD;
    }
    //block the process
    parent->s_flags |= WAITING;
    return SUSPEND;
}
