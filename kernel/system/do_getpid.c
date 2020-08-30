/**
 * Syscall in this file: getpid
 * Input:    
 *
 * Return:     reply_res:    pid
 *             m1_i2: ppid
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:09:42
 * 
*/
#include <kernel/kernel.h>


int do_getpid(struct proc *who, struct message *m){
    m->m1_i2 = who->parent;
    return who->pid;
}

int do_getppid(struct proc* who, struct message *m){
    return who->parent;
}
