/**
 * Syscall in this file: wait
 * Input:   
 *
 * Return:  m1_i1: pid of the child process
 *          m1_i2: wstatus value
 *
 *          if no child is found, this process is blocked
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:10:38
 * 
*/
#include "../winix.h"
#include <sys/wait.h>

int do_wait(struct proc *parent, struct message *mesg){
    register struct proc *child = NULL;
    
    ptr_t* wstatus;
    int children = 0,i;

    for (i=0; i<NUM_PROCS; i++) {
        child = &proc_table[i];
        
        //if there is a child process that is zombie
        if(child->i_flags & IN_USE && child->parent == parent->proc_nr){
            if(child->i_flags & ZOMBIE){
                mesg->m1_i2 = (child->exit_status << 8) | (child->sig_status & 0x7f);
                free_slot(child);
                return child->proc_nr;
            }
            parent->wpid = child->proc_nr;
            children++;
        }
    }
    
    //Proc has no children
    if(children == 0)
        return ECHILD;
    
    //block the process
    parent->s_flags |= WAITING;
    return SUSPEND;
}
