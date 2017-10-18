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

    who->flags &= ~IN_SIG_HANDLER;
    sp = get_physical_addr(who->sp,who);

    sp += sizeof(signum) + sizeof(struct syscall_frame_comm);

    //Copy the previous pcb saved on the user stack back
    //to system proc struct, information includes registers
    //scheduling flags, and messages
    memcpy(who,sp,SIGNAL_CTX_LEN);

    //restore mask
    who->sig_mask = who->sig_mask2;

    if(signum == SIGABRT){
        who->sig_table[SIGABRT].sa_handler = SIG_DFL;
        sig_proc(who, SIGABRT);
        goto end;
    }

    //Normall after invoking a system call, the process
    //will be blocked on RECEIVING reply from the system
    //But since we have restored the pcb context before
    //the signal handler, so double check process state,
    //add it to scheduling queue if necessary
    if(!who->state)
        enqueue_schedule(who);

    if(who->sig_pending)
        check_sigpending(who);
    
end:
    if(who->state & RECEIVING)
        return EINTR;
    return DONTREPLY;
}
