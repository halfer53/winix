/**
 * Process scheduling routines for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include "winix.h"
#include <sys/syscall.h>

//Linked lists are defined by a head and tail pointer.
#define HEAD 0
#define TAIL 1

//Process table
proc_t proc_table[NUM_PROCS];

//Scheduling queues
 proc_t *ready_q[NUM_QUEUES][2];

//Entries in the process table that are not in use
static proc_t *free_proc[2];

//The currently-running process
proc_t *current_proc;

//OLD Process Stacks
//static size_t proc_stacks[NUM_PROCS][DEFAULT_STACK_SIZE];

//Limits for memory allocation
size_t FREE_MEM_BEGIN = 0;
size_t FREE_MEM_END = 0;


/**
 * Adds a proc to the tail of a list.
 *
 * Parameters:
 *   q		An array containing a head and tail pointer of a linked list.
 *   proc	The proc struct to add to the list.
 **/
 void enqueue_tail(proc_t **q, proc_t *proc) {
	if(q[HEAD] == NULL) {
		q[HEAD] = q[TAIL] = proc;
	}
	else {
		q[TAIL]->next = proc;
		q[TAIL] = proc;
	}
	proc->next = NULL;
}

/**
 * Adds a proc to the head of a list.
 *
 * Parameters:
 *   q		An array containing a head and tail pointer of a linked list.
 *   proc	The proc struct to add to the list.
 **/
 void enqueue_head(proc_t **q, proc_t *proc) {
	proc_t *curr = NULL;
	// if ((proc->proc_index == 3 || proc->proc_index == 4 )&& print) {
	// 	curr = q[HEAD];
	// 	kprintf("%d before enqueue head ",proc->proc_index);
	// 	while (curr != NULL) {
	// 		kprintf("%s ",curr->name);
	// 		curr = curr->next;
	// 	}
	// 	kprintf("\n");
	// }
	if(q[HEAD] == NULL) {
		proc->next = NULL;
		q[HEAD] = q[TAIL] = proc;
	}
	else {
		proc->next = q[HEAD];
		q[HEAD] = proc;
	}
	// if ((proc->proc_index == 3 || proc->proc_index == 4 )&& print) {
	// 	curr = q[HEAD];
	// 	kprintf("%d after enqueue head ",proc->proc_index);
	// 	while (curr != NULL) {
	// 		kprintf("%s ",curr->name);
	// 		curr = curr->next;
	// 	}
	// 	kprintf("\n");
	// }
}

/**
 * Removes the head of a list.
 *
 * Parameters:
 *   q		An array containing a head and tail pointer of a linked list.
 *
 * Returns:
 *   The proc struct that was removed from the head of the list
 *   NULL if the list is empty.
 **/
static proc_t *dequeue(proc_t **q) {
	proc_t *p = q[HEAD];

	if(p == NULL) { //Empty list
		assert(q[TAIL] == NULL, "deq: tail not null");
		return NULL;
	}

	if(q[HEAD] == q[TAIL]) { //Last item
		q[HEAD] = q[TAIL] = NULL;
	}
	else { //At least one remaining item
		q[HEAD] = p->next;
	}
	p->next = NULL;
	return p;
}

//return -1 if nothing found
static int delete(proc_t **q, proc_t *h){
	proc_t *curr = q[HEAD];
	proc_t *prev = NULL;

	if(curr == NULL) { //Empty list
		assert(q[TAIL] == NULL, "delete: tail not null");
		return -1;
	}

	while(curr != h && curr != NULL){
		prev = curr;
		curr = curr->next;
	}
	if (curr != NULL) {
		if (prev == NULL) {
			q[HEAD] = q[TAIL] = NULL;
		}else{
			prev->next = curr->next;
		}
		return 1;
	}else{
		return -1;
	}

}

void add_to_scheduling_queue(proc_t* p){
	enqueue_tail(ready_q[p->priority], p);
}

proc_t *get_free_proc() {
	int i;
	proc_t *p = dequeue(free_proc);
	size_t *sp = NULL;

	if(p) {

		proc_set_default(p);
		//malloced_sp
	}
	return p;
}

void proc_set_default(proc_t *p){
	int i = 0;
	for(i = 0; i < NUM_REGS; i++) {
		p->regs[i] = DEFAULT_REG_VALUE;
	}

	p->sp = DEFAULT_STACK_POINTER;
	p->ra = DEFAULT_RETURN_ADDR;
	p->pc = DEFAULT_PROGRAM_COUNTER;
	p->rbase = DEFAULT_RBASE;
	p->ptable = DEFAULT_PTABLE;
	p->cctrl = DEFAULT_CCTRL;

	p->priority = 0;
	p->quantum = DEFAULT_QUANTUM;
	p->ticks_left = 0;
	p->time_used = 0;
	//strcpy(p->name,"Unkonwn Name");
	p->state = INITIALISING;
	p->flags = DEFAULT_FLAGS;

	p->sender_q = NULL;
	p->next_sender = NULL;
	p->message = NULL;

	p->length = 0;
	p->parent_proc_index = 0;
	p->heap_break = NULL;

	//data not initialised
	//unsigned long protection_table[PROTECTION_TABLE_LEN]
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
 proc_t *pick_proc() {
	int i;

	//Find the highest-priority non-empty queue
	for(i = 0; i < NUM_QUEUES; i++) {
		if(ready_q[i][HEAD] != NULL) {
			return dequeue(ready_q[i]);
		}
	}

	return NULL;
}



proc_t *kernel_fork_proc(proc_t *original){
  int pindex = 0;
	proc_t *p_fork = NULL;

  pindex = fork_proc(original);
	p_fork = get_proc(pindex);
  p_fork->next = NULL;
	enqueue_tail(ready_q[p_fork->priority], p_fork);
  return p_fork;
}
/**
 * fork the calling process
 *
 * Returns:
 *   proc_index of the newly forked process
 *
 * Side Effects:
 *   a new process forked onto the a new memory space, but not yet added to the scheduling queue
 **/
int fork_proc(proc_t *original){
	proc_t *p = NULL;
	void *ptr_base = NULL;
	int len =0;
	int nstart = 0;
	int pbak ;
	int length;
	int i,n,index;

	if (original->length == 0 || (size_t)(original->rbase) == 0) {
		//we can't fork p1 if it's a system task
		kprintf("%s can't be forked\n",original->name );
		return -1;
	}

	if(p = get_free_proc()) {
		pbak = p->proc_index;
		*p = *original;
		p->proc_index = pbak;
		
		len = physical_len_to_page_len(original->length + DEFAULT_STACK_SIZE);

		ptr_base = proc_malloc(original->length + DEFAULT_STACK_SIZE);

		memcpy(ptr_base,original->rbase,original->length + DEFAULT_STACK_SIZE);
		p->rbase = ptr_base;

		//Initialise protection table
		//reset page table
		p->ptable = p->protection_table;
		bitmap_reset(p->ptable,PROTECTION_TABLE_LEN);

		//get page table starting index, and its length
		nstart = get_page_index(p->rbase);
		bitmap_set_nbits(p->ptable,PROTECTION_TABLE_LEN, nstart,len);

		strcpy(p->name,"fork_");
 	 	strcat(p->name,original->name);

		//Set the process to runnable, and enqueue it.
		p->state = RUNNABLE;

		p->parent_proc_index = original->proc_index;
	}
	assert(p != NULL, "Fork");
	return p->proc_index;
}

/**
 * Creates a new process and adds it to the runnable queue
 *
 * Parameters:
 *   entry		A pointer to the entry point of the new process.
 *   priority	The scheduling priority of the new process.
 *   name		The name of the process, up to PROC_NAME_LEN characters long.
 *
 * Returns:
 *   The newly-created proc struct.
 *   NULL if the priority is not valid.
 *   NULL if the process table is full.
 *
 * Side Effects:
 *   A proc is removed from the free_proc list, reinitialised, and added to ready_q.
 */
proc_t *new_proc(void (*entry)(), int priority, const char *name) {
	proc_t *p = NULL;
	int i;
	size_t *ptr = NULL;
	int n = 0;
	int temp =0;

	//Is the priority valid?
	if(!(0 <= priority && priority < NUM_QUEUES)) {
		return NULL;
	}

  /**
   * Gets a proc struct that isn't currently in use.
   *
   * Returns:
   *   A pointer to a proc struct that isn't in use.
   *   NULL if there are no free slots in the process table.
   *
   * Side Effects:
   *   A proc struct is removed from the free_proc list, and reinitialised.
   **/
	if(p = get_free_proc()) {
		p->priority = priority;
		p->pc = entry;

		strcpy(p->name,name);


		p->ptable = p->protection_table;
		for(i = 0; i < PROTECTION_TABLE_LEN; i++) {
			p->protection_table[i] = 0xffffffff;
		}
		//system task has access to everywhere in the memory

		ptr = (size_t *)expand_mem(DEFAULT_STACK_SIZE);
		p->sp = ptr + (size_t)DEFAULT_STACK_SIZE-1;

		//Set the process to runnable, and enqueue it.
		p->state = RUNNABLE;
		enqueue_tail(ready_q[priority], p);
	}

	return p;
}

/**
 * Exits a process, and frees its slot in the process table.
 *
 * Note:
 *   The process must not currently belong to any linked list.
 *
 * Side Effects:
 *   Process state is set to DEAD, and is returned to the free_proc list.
 **/
void end_process(proc_t *p) {

	 p->state = DEAD;
	 enqueue_tail(free_proc, p);
}

//print out the list of processes currently in the ready_q
//and the currently running process
//return 0;

int process_overview(){
	int i=0;
	proc_t *curr = NULL;
	//kprintf("|| PS %s ",current_proc->name );
	for (i=0; i < NUM_QUEUES; i++) {
		if (ready_q[i][HEAD] != NULL) {
			curr = ready_q[i][HEAD];
			while(curr != NULL){
				printProceInfo(curr);
        //kprintf("%s ",curr->name);
				curr = curr->next;
			}
		}
	}
  //kprintf("||\n" );
	return 0;
}

//print the process state given
void printProceInfo(proc_t* curr){
	kprintf("name %s, i %d, rbase %x, length %d, pc %x, sp %x, state %s\r\n",curr->name, curr->proc_index, curr->rbase, curr->length,curr->pc,curr->sp,getStateName(curr->state));
}

//return the strign value of state name give proc_state_t state
char* getStateName(proc_state_t state){
	switch (state) {
		case DEAD: return "DEAD";
		case INITIALISING: return "INITIALISING";
		case RUNNABLE: return "RUNNABLE";
		case ZOMBIE: return "ZOMBIE";
		default: return "none";
	}
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
 // #define minlength  180
 // #define maxlength  195
void sched() {

	proc_t *curr = ready_q[3][HEAD];
	int nextpick = 0;
  int count = 0;
  // if (system_uptime < maxlength && system_uptime > minlength && current_proc->proc_index != 2  ) {
  //   kprintf("$ sched %d flags %d ",current_proc->proc_index,current_proc->flags);
  // }
	if(current_proc != NULL && !current_proc->flags) {
		//Accounting

		current_proc->time_used++;

		//If there's still time left, reduce timeslice and add it to the head of its priority queue
		if(--current_proc->ticks_left) {
			enqueue_head(ready_q[current_proc->priority], current_proc);
      // if (system_uptime < maxlength && system_uptime > minlength && current_proc->proc_index != 2  ) {
			// 	kprintf(" %d head enqueue ",current_proc->proc_index);
			// }
		}
		else { //Re-insert process at the tail of its priority queue
			enqueue_tail(ready_q[current_proc->priority], current_proc);
      // if (system_uptime < maxlength && system_uptime > minlength && current_proc->proc_index != 2  ) {
			// 	kprintf(" %d tail enqueue ",current_proc->proc_index);
			// }
		}
	}else{
    // if (system_uptime < maxlength && system_uptime > minlength && current_proc->proc_index != 2  ) {
    //   kprintf(" curr running %d flag %d time %d left ",current_proc->proc_index,current_proc->flags,system_uptime);
    //   while(curr != NULL && count <5){
    //
    //     kprintf("%s ",curr->name);
    //     if (curr->proc_index == curr->next->proc_index) {
    //       kprintf("loop at %d",current_proc->proc_index);
    //     }
    //     count++;
    //     curr = curr->next;
    //   }
    //   nextpick = 1;
    // }
  }

	//Get the next task
	current_proc = pick_proc();


	assert(current_proc != NULL, "sched: current_proc null");
	//Reset quantum if needed
	if(current_proc->ticks_left <= 0) {
		current_proc->ticks_left = current_proc->quantum;
  }

  // if (system_uptime < maxlength && system_uptime > minlength && current_proc->proc_index != 2  ) {
  //   kprintf(" %d picked |",current_proc->proc_index);
  // }else if (nextpick){
  //   kprintf("next pick %d ||",current_proc->proc_index);
  // }


	//Load context and run
	wramp_load_context();
}

/**
 * Gets a pointer to a process.
 *
 * Parameters:
 *   proc_nr		The process to retrieve.
 *
 * Returns:			The relevant process, or NULL if it does not exist.
 **/
proc_t *get_proc(int proc_nr) {
	if(0 <= proc_nr && proc_nr < NUM_PROCS) {
		proc_t *p = &proc_table[proc_nr];
		if(p->state != DEAD)
			return p;
	}
	return NULL;
}

/**
 * Receives a message.
 *
 * Parameters:
 *   m				Pointer to write the message to.
 *
 * Returns:
 *   0 on success
 *   -1 if destination is invalid
 **/
int wini_send(int dest, message_t *m) {
	proc_t *pDest;

	current_proc->message = m; //save for later

	//Is the destination valid?
	if(pDest = get_proc(dest)) {

		//kprintf("%d ",pDest->proc_index);
		//If destination is waiting, deliver message immediately.
		if(pDest->flags & RECEIVING) {
			//kprintf("data sent from %d\n",current_proc->proc_index);
			//Copy message to destination
			*(pDest->message) = *m;
			//Unblock receiver
			pDest->flags &= ~RECEIVING;
			enqueue_head(ready_q[pDest->priority], pDest);
      //kprintf("$at send %d enqueued curr %d type %d | ",dest,current_proc->proc_index,m->type );

		}


		else {
			//Otherwise, block current process and add it to head of sending queue of the destination.
			current_proc->flags |= SENDING;
			current_proc->next_sender = pDest->sender_q;
			pDest->sender_q = current_proc;
      //kprintf("$at send dest %d sender_q set curr %d type %d | ",dest,current_proc->proc_index,m->type);
		}

		return 0;
	}else{
		//kprintf("pid %d not found from %d\n",dest,current_proc->proc_index);
	}

	return -1;
}

/**
 * Receives a message.
 *
 * Parameters:
 *   m				Pointer to write the message to.
 *
 * Returns:			0
 **/
int wini_receive(message_t *m) {
	proc_t *p = current_proc->sender_q;

	//If a process is waiting to send to this process, deliver it immediately.
	if(p != NULL) {

		//Dequeue head node
		current_proc->sender_q = p->next_sender;

		//Copy message to this process
		*m = *(p->message);

		//Unblock sender
		p->flags &= ~SENDING;
		enqueue_head(ready_q[p->priority], p);
		//kprintf("$at receive sender %d enqueued || ",p->proc_index);
	}
	else {
		current_proc->message = m;
		current_proc->flags |= RECEIVING;
	}
	return 0;
}
/**
 * send a message to the destination, but only attempt once
 *
 * Parameters:
 *   m				Pointer to write the message to.
 *
 * Returns:
 *   0 on success
 *   -1 if destination is invalid
 **/
int wini_sendonce(int dest, message_t *m) {
	proc_t *pDest;

	current_proc->message = m; //save for later

	//Is the destination valid?
	if(pDest = get_proc(dest)) {

		//If destination is waiting, deliver message immediately.
		if(pDest->flags & RECEIVING) {

			//Copy message to destination
			*(pDest->message) = *m;

			//Unblock receiver
			pDest->flags &= ~RECEIVING;
			enqueue_head(ready_q[pDest->priority], pDest);
			//if the destination rejects any message it receives,
			//do not deliver the message, but add it to the scheduling queue
		}else{
      enqueue_tail(ready_q[current_proc->priority], current_proc);
    }

		//do nothing if it's not waiting
		return 0;
	}
	return -1;
}

/**
 * Initialises the process table
 *
 * Side Effects:
 *   ready_q is initialised to all empty queues.
 *   free_proc queue is initialised and filled with processes.
 *   proc_table is initialised to all DEAD processes.
 *   current_proc is set to NULL.
 **/
void init_proc() {
	int i;
	//Initialise queues
	// proc_t arr[2];
	// int size = (char*)&arr[1] - (char*)&arr[0];
	// kprintf("sizeof proc_t %d\n",size );

	for(i = 0; i < NUM_QUEUES; i++) {
		ready_q[i][HEAD] = NULL;
		ready_q[i][TAIL] = NULL;
	}

	free_proc[HEAD] = free_proc[TAIL] = NULL;
	//Add all proc structs to the free list
	for(i = 0; i < NUM_PROCS; i++) {
		proc_t *p = &proc_table[i];
		p->state = DEAD;

		enqueue_tail(free_proc, p);
		proc_table[i].proc_index = i;
	}

	//No current process yet.
	current_proc = NULL;
}

int sizeof_proc_t(){
	hole_t arr[2];
	int size = (char*)&arr[1] - (char*)&arr[0];
	return size;
}
