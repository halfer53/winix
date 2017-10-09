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
#include <kernel/exception.h>
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

    //Is the destination valid?
    if (pDest = get_running_proc(dest)) {
        
        if (pDest->s_flags & RECEIVING) {
            *(pDest->message) = *m;

            //Unblock receiver
            pDest->s_flags &= ~RECEIVING;
            enqueue_head(ready_q[pDest->priority], pDest);
            
        }else {
            
            //Otherwise, block current process and add it to head of sending queue of the destination.
            current_proc->s_flags |= SENDING;
            current_proc->next_sender = pDest->sender_q;
            pDest->sender_q = current_proc;
        }

        if(is_debugging_syscall()){
            if(dest == SYSTEM)
                kprintf("\nSyscall %d from %d|", m->type, m->src);
        }else if(get_debug_ipc_count()){
            kprintf("\nIPC: SEND to %d from %d type %d| ",dest, current_proc->proc_nr,m->type);
        }
        return OK;
    }
    return ERR;
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
    struct proc *p = current_proc->sender_q;
    
    //If a process is waiting to send to this process, deliver it immediately.
    if (p != NULL) {
        
        //Dequeue head node
        current_proc->sender_q = p->next_sender;
        

        //Copy message to this process
        *m = *(p->message);

        //Unblock sender
        p->s_flags &= ~SENDING;
        if(! p->s_flags && p->i_flags & RUNNABLE)
            enqueue_head(ready_q[p->priority], p);
        
        if(get_debug_ipc_count()){
            kprintf("\nIPC: %d REC from %d type %d| ",current_proc->proc_nr, m->src ,m->type);
        }
        return OK;
    }
    else if(current_proc->notify_pending != 0){
        int i;
        unsigned int map = current_proc->notify_pending;
        for(i = 0; i < 32; i++){
            if(map & (1 << i)){
                unset_bit(current_proc->notify_pending, i);
                *m = *(get_proc(i)->message);
                return OK;
            }
        }
    }

    current_proc->message = m;
    current_proc->s_flags |= RECEIVING;
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
int do_notify(int dest, struct message *m) {
    struct proc *pDest;

    current_proc->message = m; //save for later

    //Is the destination valid?
    if (pDest = get_running_proc(dest)) {

        if(get_debug_ipc_count())
                kprintf("\nNOTIFY %d from %d reply %d| ",dest, current_proc->proc_nr,m->type);
            
        //If destination is waiting, deliver message immediately.
        if (pDest->s_flags & RECEIVING) {

            //Copy message to destination
            *(pDest->message) = *m;

            //Unblock receiver
            pDest->s_flags &= ~RECEIVING;
            enqueue_head(ready_q[pDest->priority], pDest);
        }else{
            set_bit(pDest->notify_pending, current_proc->proc_nr);
        }
        //do nothing if it's not waiting
        return OK;
    }
    return ERR;
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
    return do_notify(dest,m);
}

//send used by interrupt
int interrupt_send(int dest, struct message* pm){
    if(!in_interrupt())
        return ERR;

    //curr proc is the process that generated exception
    //most likely segmentation fault or float fault
    current_proc->s_flags |= RECEIVING;
    return do_send(dest, pm);
}
