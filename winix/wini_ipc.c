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

    current_proc->message = m; //save for later
    pDest = IS_USER_PROC(current_proc) ? get_proc_by_pid(dest) : get_runnable_proc(dest);
    
    //Is the destination valid?
    if (pDest) {

        //if the destination is waiting for the curr proc
        //avoid deadlock
        if(pDest->state & SENDING){
            struct proc* xp = current_proc->sender_q;
            while(xp){
                if(xp == pDest){
                    return EDEADLK;
                }
                xp = xp->next_sender;
            }
        }
        
        if (pDest->state & RECEIVING) {
            *(pDest->message) = *m;

            //Unblock receiver
            pDest->state &= ~RECEIVING;
            pDest->regs[0] = m->reply_res;
            enqueue_head(ready_q[pDest->priority], pDest);
        }else {
            
            //Otherwise, block current process and add it to
            // head of sending queue of the destination.
            current_proc->state |= SENDING;
            current_proc->next_sender = pDest->sender_q;
            pDest->sender_q = current_proc;
        }

        if(is_debugging_syscall()){
            if(dest == SYSTEM)
                kprintf_syscall_request(m->type, m->src);
        }else if(is_debugging_ipc()){
            kprintf("\nIPC: SEND to %d from %d type %d| ",
                        dest, current_proc->proc_nr,m->type);
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
    struct proc *p;

    if(current_proc->notify_pending){
        int i;
        unsigned int map = current_proc->notify_pending;
        for(i = 0; i < 32; i++){
            if(map & (1 << i)){
                unset_bit(current_proc->notify_pending, i);
                i = SID_TO_TASK_NR(i);
                *m = i == INTERRUPT ? *get_exception_m() : *(get_proc(i)->message);
                return OK;
            }
        }
    }
    
    p = current_proc->sender_q;
    //If a process is waiting to send to this process, deliver it immediately.
    if (p != NULL) {
        
        //Dequeue head node
        current_proc->sender_q = p->next_sender;
        

        //Copy message to this process
        *m = *(p->message);

        //Unblock sender
        p->state &= ~SENDING;
        if(!p->state && p->flags & RUNNABLE)
            enqueue_head(ready_q[p->priority], p);
        
        if(is_debugging_ipc()){
            kprintf("\nIPC: %d REC from %d type %d| ",current_proc->proc_nr, m->src ,m->type);
        }
        return OK;
    }

    current_proc->message = m;
    current_proc->state |= RECEIVING;
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

    //Is the destination valid?
    if (pDest = get_runnable_proc(dest)) {

        if(is_debugging_syscall()){
            if(IS_USER_PROC(pDest))
                kprintf_syscall_reply(m->reply_res);
        }else if(is_debugging_ipc())
            kprintf("\nNOTIFY %d from %d type %d| ",dest, src ,m->type);
            
        //If destination is waiting, deliver message immediately.
        if (pDest->state == RECEIVING) {

            //Copy message to destination
            *(pDest->message) = *m;

            //Unblock receiver
            pDest->state &= ~RECEIVING;
            pDest->regs[0] = m->reply_res;
            enqueue_head(ready_q[pDest->priority], pDest);
        }else{
            int sid = TASK_NR_TO_SID(src);
            set_bit(pDest->notify_pending, sid);
        }
        //do nothing if it's not waiting
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

int syscall_reply(int reply, int dest,struct message* m){
    m->reply_res = reply;
    return do_notify(SYSTEM, dest,m);
}

