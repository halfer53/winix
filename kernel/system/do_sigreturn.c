/**
 * Syscall in this file: sigreturn
 * Input:   m1_i1: signum number
 *
 * Return:  This syscall does not return
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:10:17
 * 
*/
#include <kernel/kernel.h>
#include <winix/signal.h>

/**
 * This method resume the system call if it was previously interruppted
 * currently, it just simply return -1 to the user space, and set errno 
 * to EINTR to indicate failure
 * @param to user process to which we send err to
 */
 int resume_from_sig(struct proc *to){
    struct syscall_ctx* prev_syscall;

    prev_syscall = interrupted_syscall_ctx();

    if(prev_syscall->interruptted && to->s_flags & RECEIVING &&
        prev_syscall->who->proc_nr == to->proc_nr){
        
        //return EINTR to the user
        syscall_reply(EINTR, to->proc_nr,&prev_syscall->m);

    }else{
        // kprintf("enqueue %d pri %d\n",to->proc_nr, to->priority);
        enqueue_schedule(to);
    }
    memset(prev_syscall, 0, sizeof(struct syscall_ctx));
    return OK;
}

int do_sigreturn(struct proc *who, struct message *m){
    reg_t *sp;
    struct proc *systask;
    int signum = m->m1_i1;

    // debug_ipc(4); //debug
    sp = get_physical_addr(who->sp,who);

    sp += sizeof(struct sigframe);
    //Copy the previous pcb saved on the user stack back
    //to system proc struct, information includes registers
    //scheduling flags, and messages
    memcpy(who,sp,SIGNAL_CTX_LEN);

    //reset the signal to default
    who->sig_table[signum].sa_handler = SIG_DFL;
    resume_from_sig(who);

    return DONTREPLY;
}
