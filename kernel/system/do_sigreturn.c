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
#include <winix/sigsend.h>

int do_sigreturn(struct proc *who, struct message *m){
    reg_t *sp;
    struct proc *systask;
    int signum = m->m1_i1;

    sp = get_physical_addr(who->ctx.m.sp,who);

    sp += sizeof(struct syscall_frame_comm);

    // Copy the previous pcb saved on the user stack back
    // to system proc struct, information includes registers
    // scheduling flags, and messages
    memcpy(who,sp,SIGNAL_CTX_LEN);

    // restore mask
    who->sig_mask = who->sig_mask2;

    if(signum == SIGABRT){
        who->sig_table[SIGABRT].sa_handler = SIG_DFL;
        send_sig(who, SIGABRT);
        goto end;
    }

    // Normall after invoking a system call, the process
    // will be blocked on STATE_RECEIVING reply from the system
    // But since we have restored the pcb context before
    // the signal handler, so double check process state,
    // add it to scheduling queue if necessary
    if(who->state == STATE_RUNNABLE)
        enqueue_schedule(who);
    
end:
    /**
     * This is a nasty corner case here, if the signal handler
     * is invoked while the process is blocked e.g. by wait(2),
     * the state of the blocked process would be
     *  (STATE_RECEIVING | STATE_WAITING).
     * if we would simply do syscall_reply(EINTR, pnr, m), then
     * only STATE_RECEIVING is cleared, STATE_WAITING is not. Since
     * a process is only runnable if state == 0, the user space still
     * wouldn't get the response.
     * 
     * since we want to return EINTR to the user, thus state has to be
     * cleared, or equal to STATE_RUNNABLE, thus both RECEIVING AND WAITING
     * need to be cleared. 
     * 
     * by making who->state = STATE_RECEIVING, we are ensuring that EINTR
     * is returned to the user space, and no other state info is blocking our way
     */
    if(who->state & STATE_RECEIVING){
        who->state = STATE_RECEIVING;
        return EINTR;
    }
        
    return DONTREPLY;
}
