/**
 * Syscall in this file: winfo
 * NB this is a winix specific system call
 * Input:   i1: type of information to be displayed
 *
 * Return:  i1: OK
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
#include "../winix.h"

int do_winfo(struct proc *who, struct message *m){
    switch(m->i1){
        case WINFO_PS:
            print_runnable_procs();
            break;

        case WINFO_MEM:
            print_sysmap();
            break;
        default:
            break;
    }
    return OK;
}

