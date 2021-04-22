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
    struct filp* file = who->fp_filp[STDOUT_FILENO];

    if(!file)
        return EINVAL;

    switch(m->m1_i1){
        case WINFO_PS:
            kreport_all_procs(file);
            break;

        case WINFO_SLAB:
            kprint_slab();
            break;

        case WINFO_MEM:
            kreport_sysmap();
            break;

        case WINFO_TRACE_SYSCALL:
            trace_syscall = true;
            break;

        case WINFO_DISABLE_TRACE:
            trace_syscall = false;
            break;

        case WINFO_DEBUG_IPC:
            debug_ipc(999);
            break;
        
        case WINFO_DEBUG_SCHEDULING:
            debug_scheduling(m->m1_i2);
            break;

        default:
            return EINVAL;
    }
    return OK;
}

