#include "../winix.h"

/**
 * winix specific syscall for displaying system information
 * @param  who 
 * @param  m   
 * @return     
 */
int do_winfo(struct proc *who, struct message *m){
    switch(m->i1){
        case WINFO_PS:
            process_overview();
            break;

        case WINFO_MEM:
            print_sysmap();
            break;
        default:
            break;
    }
    return OK;
}

