#include "../winix.h"

/**
 * Syscall in this file: winfo
 * NB this is a winix specific system call
 * Input:   i1: type of information to be displayed
 *
 * Return:  i1: OK
 * 
 */
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

