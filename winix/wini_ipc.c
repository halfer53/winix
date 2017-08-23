/**
 * 
 * Winix essential ipcs
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * @create date 2016-09-19
 * @modify date 2017-08-23 06:01:38
*/
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

		if(get_debug_ipc_count()){
			if(dest == SYSTEM_TASK){
				kprintf("\nSyscall %d from %d|", m->type, m->src);
			}else if(current_proc->proc_nr != SYSTEM_TASK){
				kprintf("\nSEND %d flags %d from %d t %d| ",dest, pDest->s_flags, current_proc->proc_nr,m->type);
			}
		}
		
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

		if(get_debug_ipc_count()){
			if(current_proc->proc_nr == SYSTEM_TASK)
				kprintf("\nSyscall %d from %d|", m->type, m->src);
			else
				kprintf("\nREC from %d t %d| ",p->proc_nr ,m->type);
		}
			
		
		//Dequeue head node
		current_proc->sender_q = p->next_sender;

		//Copy message to this process
		*m = *(p->message);

		//Unblock sender
		p->s_flags &= ~SENDING;
		enqueue_head(ready_q[p->priority], p);
	}
	else {
		current_proc->message = m;
		current_proc->s_flags |= RECEIVING;
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

		if(get_debug_ipc_count())
				kprintf("\nNOTIFY %d flags %d from %d t %d| ",dest, pDest->s_flags, current_proc->proc_nr,m->type);
			
		//If destination is waiting, deliver message immediately.
		if (pDest->s_flags & RECEIVING) {

			//Copy message to destination
			*(pDest->message) = *m;

			//Unblock receiver
			pDest->s_flags &= ~RECEIVING;
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
	memset(&m,-1,sizeof( struct message));
	return winix_send(dest,&m);
}
