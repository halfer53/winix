#include <kernel/kernel.h>
/**
 * sends a message.
 *
 * Parameters:
 *   m				Pointer to write the message to.
 *
 * Returns:
 *   0 on success
 *   -1 if destination is invalid
 **/

int wini_send(int dest, struct message *m) {
	struct proc *pDest;

	current_proc->message = m; //save for later
	
	//Is the destination valid?
	if (pDest = get_running_proc(dest)) {

		if(DEBUG_IPC){
			kprintf("\nSEND %d flags %d from %d t %d| ",dest, pDest->flags, current_proc->proc_nr,m->type);
			DEBUG_IPC--;
		}

		if (pDest->flags & RECEIVING) {
			*(pDest->message) = *m;
			//Unblock receiver
			pDest->flags &= ~RECEIVING;
			enqueue_head(ready_q[pDest->priority], pDest);
			
		}else {
			
			//Otherwise, block current process and add it to head of sending queue of the destination.
			current_proc->flags |= SENDING;
			current_proc->next_sender = pDest->sender_q;
			pDest->sender_q = current_proc;
		}

		return OK;
	}
	return ERR;
}

/**
 * Receives a message.
 *
 * Parameters:
 *   m				Pointer to write the message to.
 *
 * Returns:			0
 **/
int wini_receive(struct message *m) {
	struct proc *p = current_proc->sender_q;

	//If a process is waiting to send to this process, deliver it immediately.
	if (p != NULL) {

		if(DEBUG_IPC){
			kprintf("\nREC from %d t %d| ",p->proc_nr ,m->type);
			DEBUG_IPC--;
		}
		//Dequeue head node
		current_proc->sender_q = p->next_sender;

		//Copy message to this process
		*m = *(p->message);

		//Unblock sender
		p->flags &= ~SENDING;
		enqueue_head(ready_q[p->priority], p);
	}
	else {
		current_proc->message = m;
		current_proc->flags |= RECEIVING;
	}
	return OK;
}
/**
 * non-block send
 *
 * Parameters:
 *   m				Pointer to write the message to.
 *
 * Returns:
 *   0 on success
 *   -1 if destination is invalid
 **/
int wini_notify(int dest, struct message *m) {
	struct proc *pDest;

	current_proc->message = m; //save for later

	//Is the destination valid?
	if (pDest = get_running_proc(dest)) {

		if(DEBUG_IPC){
			kprintf("\nNOTIFY %d flags %d from %d t %d| ",dest, pDest->flags, current_proc->proc_nr,m->type);
			DEBUG_IPC--;
		}

		//If destination is waiting, deliver message immediately.
		if (pDest->flags & RECEIVING) {

			//Copy message to destination
			*(pDest->message) = *m;

			//Unblock receiver
			pDest->flags &= ~RECEIVING;
			enqueue_head(ready_q[pDest->priority], pDest);
			//if the destination rejects any message it receives,
			//do not deliver the message, but add it to the scheduling queue
		}

		//do nothing if it's not waiting
		return OK;
	}
	return ERR;
}

/**
 * send err to the destination
 * @param  dest 
 * @return      
 */
int winix_senderr(int dest){
	struct message m;
	memset(&m,-1,MESSAGE_LEN);
	return winix_send(dest,&m);
}
