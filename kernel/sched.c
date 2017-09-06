/**
 * 
 * Winix Scheduling module
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
#include "winix.h"

void rebalance_queues(int proc_nr, clock_t time){
	struct proc* curr;
	int i;
	for(i = 0; i < NUM_PROCS; i++){
		curr = &proc_table[i];
		if(! IS_IDLE(curr)){
			curr->priority = MAX_PRIORITY;
		}
	}
	new_timer(REBALANCE_TIMEOUT, rebalance_queues);
}

/**
 * Chooses a process to run.
 *
 * Returns:
 *   The process that is runnable with the highest priority.
 *   NULL if no processes are runnable (should never happen).
 *
 * Side Effects:
 *   A proc is removed from a ready_q.
 **/
struct proc *pick_proc() {
	int i;

	//Find the highest-priority non-empty queue
	for (i = 0; i < NUM_QUEUES; i++) {
		if (ready_q[i][HEAD] != NULL) {
			return dequeue(ready_q[i]);
		}
	}

	PANIC("No procs left");
	return NULL;
}


/**
 * The Scheduler.
 *
 * Notes:
 *   Context of current_proc must already be saved.
 *   If successful, this function does not return.
 *
 * Side Effects:
 *   current_proc has its accounting fields updated, and is reinserted to ready_q.
 *   current_proc is updated to point to the next runnable process.
 *   Context of the new proc is loaded.
 **/
void sched() {
	//irq count is increased for each exception being called, and cleared on exiting
	//exception
	reset_irq_count();

	//Check exception stack
	if(*(get_exception_stack_top()) != STACK_MAGIC)
		PANIC("Exception stack overflow\n");

	if (current_proc != NULL && !current_proc->s_flags) {
		//Accounting
		current_proc->time_used++;

		if (--current_proc->ticks_left) {
			enqueue_head(ready_q[current_proc->priority], current_proc);
		}
		else { //Re-insert process at the tail of its priority queue
			if(current_proc->priority < NUM_QUEUES - 1){
				current_proc->priority++;
			}
			enqueue_tail(ready_q[current_proc->priority], current_proc);
		}
	}

	current_proc = pick_proc();
	if(get_debug_sched_count())
		kprintf("| %d |",current_proc->proc_nr);

	//Reset quantum if needed
	if (current_proc->ticks_left <= 0) {
		current_proc->ticks_left = current_proc->quantum;
	}

	//Load context and run
	wramp_load_context();
}

