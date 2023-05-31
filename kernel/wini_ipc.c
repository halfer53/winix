/**
 * 
 * Winix essential ipcs
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/

#include <kernel/kernel.h>
#include <kernel/table.h>
#include <kernel/exception.h>
#include <winix/bitmap.h>
#include <winix/list.h>

/**
 * sends a message.
 *
 * Parameters:
 *   m                Pointer to write the message to.
 *
 * Returns:
 *   0 on success
 *   -1 if destination is invalid
 **/

int do_send(struct proc* src, int dest, struct message *m) {
    struct proc *pDest;

    src->message = m; // save for later
    pDest = get_proc(dest);
    
    // Is the destination valid?
    if (pDest) {

        // if the destination is waiting for the curr proc
        // avoid deadlock
        if(pDest->state & STATE_SENDING){
            struct proc* xp = src->sender_q;
            while(xp){
                if(xp == pDest){
                    return -EDEADLK;
                }
                xp = xp->next_sender;
            }
        }
        
        if (pDest->state == STATE_RECEIVING) {
            *(pDest->message) = *m;

            // Unblock receiver
            pDest->state &= ~STATE_RECEIVING;
            pDest->ctx.m.regs[0] = m->reply_res;
            enqueue_head(ready_q[pDest->priority], pDest);
            if(is_debugging_ipc()){
                kdebug("IPC: msg delivered to %d from %d\n", dest, src->proc_nr);
            }
        }else {
            if(is_debugging_ipc()){
                kdebug("IPC: SEND to %d from %d blocked\n",
                            dest, src->proc_nr);
            }
            // Otherwise, block current process and add it to
            // head of sending queue of the destination.
            src->state |= STATE_SENDING;
            src->next_sender = pDest->sender_q;
            pDest->sender_q = src;
        }

        return 0;
    }
    return -ESRCH;
}

/**
 * Receives a message.
 *
 * Parameters:
 *   m                Pointer to write the message to.
 *
 * Returns:            0
 **/
int do_receive(struct message *m) {
    struct proc *p;
    int i;

    if(curr_scheduling_proc->notify_pending){
        for(i = 0; i < 32; i++){
            if(is_bit_on(&curr_scheduling_proc->notify_pending, 1, i)){
                bitmap_clear_bit(&curr_scheduling_proc->notify_pending, 1, i);
                p = get_proc(i);
                *m = *(p->message);
                if(is_debugging_ipc()){
                    klog("%d notify queue %d type %d\n", curr_scheduling_proc->proc_nr, p->proc_nr, m->type);
                }
                return 0;
            }
        }
    }
    
    p = curr_scheduling_proc->sender_q;
    // If a process is waiting to send to this process, deliver it immediately.
    if (p != NULL) {
        
        // Dequeue head node
        curr_scheduling_proc->sender_q = p->next_sender;
        

        // Copy message to this process
        *m = *(p->message);

        // Unblock sender
        p->state &= ~STATE_SENDING;
        if(p->state == STATE_RUNNABLE)
            enqueue_head(ready_q[p->priority], p);
        
        // if(is_debugging_ipc())
        //     kdebug("IPC: %d REC from %d type %d\n",curr_scheduling_proc->proc_nr, m->src ,m->type);
        
        return 0;
    }

    curr_scheduling_proc->message = m;
    curr_scheduling_proc->state |= STATE_RECEIVING;
    return 0;
}
/**
 * non-block send
 *
 * Parameters:
 *   m                Pointer to write the message to.
 *
 * Returns:
 *   0 on success
 *   -1 if destination is invalid
 **/
int do_notify(int src, int dest, struct message *m) {
    struct proc *pDest, *pSrc;
    int syscall_num;
    const char *msg_type = "";
    // Is the destination valid?
    if ((pDest = get_proc(dest))) {

        if(pDest->state & STATE_ZOMBIE)
            return -EINVAL;

        // kdebug("\nNOTIFY %d from %d type %d| ",dest, src ,m->type);
            
        // If destination is waiting, deliver message immediately.
        if (pDest->state == STATE_RECEIVING) {

            if(pDest->flags & DIRECT_SYSCALL){
                pDest->flags &= ~DIRECT_SYSCALL;
            }

            // Set syscall reply
            (void)set_syscall_reply(pDest, m->reply_res, m->type);

            // Copy message to destination
            *(pDest->message) = *m;

            // Unblock receiver
            pDest->state &= ~STATE_RECEIVING;
            
            if(pDest->state == STATE_RUNNABLE){
                enqueue_head(ready_q[pDest->priority], pDest);
            }
        }else{
            pSrc = get_proc(src);
            if(IS_USER_PROC(pSrc)){
                pSrc->message = m;
            }else{
                syscall_num = m->type;
                if(syscall_num > 0 && syscall_num < _NSYSCALL){
                    msg_type = syscall_str[syscall_num];
                }
                kwarn("notify: dest %s[%d] state %x cant receive %s from %d\n", pDest->name, pDest->proc_nr, pDest->state, msg_type, src);
            }
        }
        // do nothing if it's not waiting
        return 0;
    }
    return -ESRCH;
}

/**
 * Non-blocking send
 *
 **/
int winix_notify(int dest, struct message *m) {
    return wramp_syscall(WINIX_NOTIFY, dest, m);
}

