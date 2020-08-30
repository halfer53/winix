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

    current_proc->message = m; // save for later
    pDest = IS_USER_PROC(current_proc) ? get_proc_by_pid(dest) : get_proc(dest);
    
    // Is the destination valid?
    if (pDest) {

        // if the destination is waiting for the curr proc
        // avoid deadlock
        if(pDest->state & STATE_SENDING){
            struct proc* xp = current_proc->sender_q;
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
        }else {
            // if(is_debugging_ipc()){
            //     KDEBUG(("\nIPC: SEND to %d from %d blocked| ",
            //                 dest, current_proc->proc_nr,m->type));
            // }
            // Otherwise, block current process and add it to
            // head of sending queue of the destination.
            current_proc->state |= STATE_SENDING;
            current_proc->next_sender = pDest->sender_q;
            pDest->sender_q = current_proc;
        }

        // if(is_debugging_ipc()){
        //     KDEBUG(("\nIPC: SEND to %d from %d type %d| ",
        //                 dest, current_proc->proc_nr,m->type));
        // }
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

    if(!list_empty(&current_proc->notify_queue)){
        // KDEBUG(("not empty"));
        list_for_each_entry_safe(struct proc, p, bak, &current_proc->notify_queue, notify_queue){
            list_del(&p->notify_queue);
            *m = *(p->message);
            // KDEBUG(("receive not %d call %d\n", p->proc_nr, m->src));
            return OK;
        }
    }
    
    p = current_proc->sender_q;
    // If a process is waiting to send to this process, deliver it immediately.
    if (p != NULL) {
        
        // Dequeue head node
        current_proc->sender_q = p->next_sender;
        

        // Copy message to this process
        *m = *(p->message);

        // Unblock sender
        p->state &= ~STATE_SENDING;
        if(p->state == STATE_RUNNABLE)
            enqueue_head(ready_q[p->priority], p);
        
        // if(is_debugging_ipc())
        //     KDEBUG(("\nIPC: %d REC from %d type %d| ",current_proc->proc_nr, m->src ,m->type));
        
        return OK;
    }

    current_proc->message = m;
    current_proc->state |= STATE_RECEIVING;
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
    int reply;
    // Is the destination valid?
    if (pDest = get_proc(dest)) {

        if(pDest->state & STATE_KILLED)
            return ERR;

        // if(is_debugging_ipc())
        //     KDEBUG(("\nNOTIFY %d from %d type %d| ",dest, src ,m->type));
            
        // If destination is waiting, deliver message immediately.
        if (pDest->state == STATE_RECEIVING) {

            if(pDest->flags & DIRECT_SYSCALL){
                if(m->reply_res < 0){
                    reply = -(m->reply_res);
                    *(USER_ERRNO(pDest)) = reply;
                    // KDEBUG(("save %d to %s %x\n", reply, pDest->name,  get_virtual_addr((pDest->stack_top + 1), pDest)));
                    m->reply_res = -1;
                }
                pDest->flags &= ~DIRECT_SYSCALL;
            }
            // Copy message to destination
            *(pDest->message) = *m;
            

            // Unblock receiver
            pDest->state &= ~STATE_RECEIVING;
            pDest->ctx.m.regs[0] = m->reply_res;
            enqueue_head(ready_q[pDest->priority], pDest);
        }else{
            pSrc = get_proc(src);
            if(IS_KERNEL_PROC(pSrc)){
                kwarn("Dest %d state %x cannot receive message from kernel\n", pDest->proc_nr, pDest->state);
                return ERR;
            }
            pSrc->message = m;
            list_add(&pSrc->notify_queue, &pDest->notify_queue);
            // KDEBUG(("notify from %d to %d, %d\n", pSrc->proc_nr, pDest->proc_nr, list_empty(&pDest->notify_queue)));
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

