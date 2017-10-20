/**
 * Syscall in this file: winfo
 * NB this is a winix specific system call
 * Input:   m1_i1: type of information to be displayed
 *
 * Return:  reply_res: OK
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:10:46
 * 
*/
#include <kernel/kernel.h>

int do_winfo(struct proc *who, struct message *m){
    switch(m->m1_i1){
        case WINFO_PS:
            kreport_all_procs();
            break;

        case WINFO_MEM:
            kreport_sysmap();
            break;
        default:
            return EINVAL;
    }
    return OK;
}

