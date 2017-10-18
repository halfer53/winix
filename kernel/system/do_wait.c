/**
 * Syscall in this file: wait
 * Input:   m1_i1: pid
 *          m1_i2: options
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

int get_wstats(struct proc* child){
    return (child->exit_status << 8) | (child->sig_status & 0x7f);
}

int do_wait(struct proc *parent, struct message *mesg){
    register struct proc *child;
    pid_t pid;
    int options;
    int children = 0;

    pid = mesg->m1_i1;
    options = mesg->m1_i2;
    
    foreach_proc(child) {
        if(child->parent == parent->proc_nr){

            if  (pid > 0 && pid != child->pid) continue;
            if  (pid < -1 && -pid != child->procgrp) continue;
            if  (pid == 0 && child->procgrp != parent->procgrp) continue;

            if(child->flags & ZOMBIE){
                mesg->m1_i2 = get_wstats(child);
                release_zombie(child);
                return child->pid;
            }

            parent->wpid = child->pid;
            children++;
        }
    }
    
    //Proc has children
    if(children > 0){

        if(options & WNOHANG)
            return OK;

        if(pid > 0)
            parent->wpid = pid;
        
        //block the process
        parent->state |= WAITING;
        return SUSPEND;
    }
    //no child
    return ECHILD;
}
