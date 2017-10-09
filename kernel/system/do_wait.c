/**
 * Syscall in this file: wait
 * Input:   
 *
 * Return:  reply_res: pid of the child process
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
#include <kernel/kernel.h>
#include <sys/wait.h>

int do_wait(struct proc *parent, struct message *mesg){
    register struct proc *child = NULL;
    
    ptr_t* wstatus;
    int children = 0,i;

    foreach_user_proc(child) {
        //if there is a child process that is zombie
        if(child->parent == parent->proc_nr){
            if(child->i_flags & ZOMBIE){
                mesg->m1_i2 = (child->exit_status << 8) | (child->sig_status & 0x7f);
                release_zombie(child);
                return child->pid;
            }
            parent->wpid = child->pid;
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
