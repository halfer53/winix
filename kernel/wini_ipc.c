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

int do_send(int dest, struct message *m) {
    struct proc *pDest;

    curr_scheduling_proc->message = m; // save for later
    pDest = get_proc(dest);
    
    // Is the destination valid?
    if (pDest) {

        // if the destination is waiting for the curr proc
        // avoid deadlock
        if(pDest->state & STATE_SENDING){
            struct proc* xp = curr_scheduling_proc->sender_q;
            while(xp){
                if(xp == pDest){
                    return EDEADLK;
                }
                xp = xp->next_sender;
            }
        }
        
        if (pDest->state & STATE_RECEIVING) {
            *(pDest->message) = *m;

            // Unblock receiver
            pDest->state &= ~STATE_RECEIVING;
            pDest->ctx.m.regs[0] = m->reply_res;
            enqueue_head(ready_q[pDest->priority], pDest);
            // if(is_debugging_ipc()){
            //     KDEBUG(("IPC: msg delivered to %d from %d\n", dest, curr_scheduling_proc->proc_nr));
            // }
        }else {
            // if(is_debugging_ipc()){
            //     KDEBUG(("IPC: SEND to %d from %d blocked\n",
            //                 dest, curr_scheduling_proc->proc_nr,m->type));
            // }
            // Otherwise, block current process and add it to
            // head of sending queue of the destination.
            curr_scheduling_proc->state |= STATE_SENDING;
            curr_scheduling_proc->next_sender = pDest->sender_q;
            pDest->sender_q = curr_scheduling_proc;
        }

        return OK;
    }
    return ESRCH;
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
    struct proc *p, *bak;
    int i, pending;

    // if(!list_empty(&curr_scheduling_proc->notify_queue)){
    //     // KDEBUG(("not empty"));
    //     list_for_each_entry_safe(struct proc, p, bak, &curr_scheduling_proc->notify_queue, notify_queue){
    //         list_del(&p->notify_queue);
    //         *m = *(p->message);
    //         // if(is_debugging_ipc()){
    //         //     KDEBUG(("%d notify queue %d type %d\n", curr_scheduling_proc->proc_nr, p->proc_nr, m->type));
    //         // }
    //         return OK;
    //     }
    // }
    if(curr_scheduling_proc->notify_pending){
        for(i = 0; i < 32; i++){
            if(is_bit_on(&curr_scheduling_proc->notify_pending, 1, i)){
                bitmap_clear_bit(&curr_scheduling_proc->notify_pending, 1, i);
                p = get_proc(i);
                *m = *(p->message);
                if(is_debugging_ipc()){
                    klog("%d notify queue %d type %d\n", curr_scheduling_proc->proc_nr, p->proc_nr, m->type);
                }
                return OK;
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
        //     KDEBUG(("IPC: %d REC from %d type %d\n",curr_scheduling_proc->proc_nr, m->src ,m->type));
        
        return OK;
    }

    curr_scheduling_proc->message = m;
    curr_scheduling_proc->state |= STATE_RECEIVING;
    return OK;
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
    int reply, syscall_num;
    char *msg_type = "";
    // Is the destination valid?
    if (pDest = get_proc(dest)) {

        if(pDest->state & STATE_KILLED)
            return ERR;


        // KDEBUG(("\nNOTIFY %d from %d type %d| ",dest, src ,m->type));
            
        // If destination is waiting, deliver message immediately.
        if (pDest->state == STATE_RECEIVING || pDest->state & STATE_STOPPED) {

            if(pDest->flags & DIRECT_SYSCALL){
                set_reply_res_errno(pDest, m);
                pDest->flags &= ~DIRECT_SYSCALL;
            }
            // Copy message to destination
            *(pDest->message) = *m;
            

            // Unblock receiver
            pDest->state &= ~STATE_RECEIVING;
            pDest->ctx.m.regs[0] = m->reply_res;
            if(pDest->state == STATE_RUNNABLE){
                enqueue_head(ready_q[pDest->priority], pDest);
            }
        }else{
            pSrc = get_proc(src);
            syscall_num = m->type;
            if(syscall_num > 0 && syscall_num < _NSYSCALL){
                msg_type = syscall_str[syscall_num];
            }
            if(IS_KERNEL_PROC(pSrc)){
                kwarn("notify: dest %s[%d] state %x cant receive %s from %d\n", pDest->name, pDest->proc_nr, pDest->state, msg_type, src);
            }
            pSrc->message = m;
            bitmap_set_bit(&pDest->notify_pending, 1, pSrc->proc_nr);
            // list_add(&pSrc->notify_queue, &pDest->notify_queue);
        }
        // do nothing if it's not waiting
        return OK;
    }
    return ESRCH;
}

/**
 * Non-blocking send
 *
 **/
int winix_notify(int dest, struct message *m) {
    return wramp_syscall(WINIX_NOTIFY, dest, m);
}

