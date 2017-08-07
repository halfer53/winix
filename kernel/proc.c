/**
 * Process scheduling routines for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include "winix.h"

//Linked lists are defined by a head and tail pointer.

//Process table
struct proc proc_table[NUM_PROCS];

//Scheduling queues
struct proc *ready_q[NUM_QUEUES][2];

//blocking queues
struct proc *block_q[2];

//Entries in the process table that are not in use
static struct proc *free_proc[2];

//The currently-running process
struct proc *current_proc;


/**
 * Adds a proc to the tail of a list.
 *
 * Parameters:
 *   q		An array containing a head and tail pointer of a linked list.
 *   proc	The proc struct to add to the list.
 **/
void enqueue_tail(struct proc **q, struct proc *proc) {
	if (q[HEAD] == NULL) {
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
void enqueue_head(struct proc **q, struct proc *proc) {
	struct proc *curr = NULL;
	if (q[HEAD] == NULL) {
		proc->next = NULL;
		q[HEAD] = q[TAIL] = proc;
	}
	else {
		proc->next = q[HEAD];
		q[HEAD] = proc;
	}
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
struct proc *dequeue(struct proc **q) {
	struct proc *p = q[HEAD];

	if (p == NULL) { //Empty list
		assert(q[TAIL] == NULL, "deq: tail not null");
		return NULL;
	}

	if (q[HEAD] == q[TAIL]) { //Last item
		q[HEAD] = q[TAIL] = NULL;
	}
	else { //At least one remaining item
		q[HEAD] = p->next;
	}
	p->next = NULL;
	return p;
}

//return ERR if nothing found
int remove_from_scheduling_queue( struct proc *h) {
	struct proc *curr;
	struct proc *prev = NULL;
	struct proc ** q = ready_q[h->priority];

	curr = q[HEAD];

	if (curr == NULL) { //Empty list
		assert(q[TAIL] == NULL, "delete: tail not null");
		return ERR;
	}

	while (curr != h && curr != NULL) {
		prev = curr;
		curr = curr->next;
	}

	if (curr == NULL)
		return ERR;

	if (prev == NULL) {
		q[HEAD] = curr->next;
		if(curr->next == NULL){
			q[TAIL] = curr->next;
		}
	} else {
		prev->next = curr->next;
	}
	return OK;
}

void add_to_scheduling_queue(struct proc* p) {
	enqueue_tail(ready_q[p->priority], p);
}

struct proc *get_free_proc_slot() {
	int i;
	struct proc *p = dequeue(free_proc);
	size_t *sp = NULL;

	if (p) {
		proc_set_default(p);
		p->IN_USE = 1;
		return p;
	}
	return NULL;
}


void proc_set_default(struct proc *p) {
	int i = 0;
	for (i = 0; i < NUM_REGS; i++) {
		p->regs[i] = DEFAULT_REG_VALUE;
	}

	p->sp = DEFAULT_STACK_POINTER;
	p->ra = DEFAULT_RETURN_ADDR;
	p->pc = DEFAULT_PROGRAM_COUNTER;
	p->rbase = DEFAULT_RBASE;
	p->ptable = DEFAULT_PTABLE;
	p->cctrl = DEFAULT_CCTRL;

	p->quantum = DEFAULT_QUANTUM;
	p->ticks_left = 0;
	p->time_used = 0;
	//strcpy(p->name,"Unkonwn");
	p->state = INITIALISING;
	p->flags = DEFAULT_FLAGS;

	p->sender_q = NULL;
	p->next_sender = NULL;
	p->message = NULL;

	p->length = 0;
	p->parent = 0;
	p->heap_break = NULL;
	p->proc_nr = p->proc_nr;

	p->ptable = p->protection_table;
	bitmap_clear(p->ptable, PTABLE_LEN);

	p->alarm.time_out = 0;
	p->alarm.next = NULL;
	p->alarm.flags = 0;
	p->alarm.proc_nr = p->proc_nr;

	memset(&p->sig_table,0,_NSIG * 3); //3: sizeof struct sigaction
}

reg_t* alloc_stack(struct proc *who){
	int page_size;
	int index;
	ptr_t *addr;

	page_size = IS_USER_PROC(who) ? USER_STACK_PAGE_SIZE : KERNEL_STACK_PAGE_SIZE;
	index = user_get_free_pages(who, page_size, __GFP_HIGH);
	if(index == ERR)
		return NULL;
	bitmap_set_bit(who->ptable, PTABLE_LEN, index - 1); 
	//set previous page to inaccessible, to prevent stack overflow from interfering other virtual address space
	addr = PAGE_ADDR(index);
	return get_virtual_addr(addr, who);
}


int set_proc(struct proc *p, void (*entry)(), int priority, const char *name) {
	p->priority = priority;
	p->pc = entry;
	strcpy(p->name, name);
	//Set the process to runnable, remember to enqueue it after you call this method
	p->state = RUNNABLE;
	return OK;
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
struct proc *start_kernel_proc(void (*entry)(), int priority, const char *name) {
	struct proc *p = NULL;
	int i;
	size_t *ptr = NULL;
	int n = 0;
	int temp = 0;

	if (!IS_PRIORITY_OK(priority)) 
		return NULL;
	
	if (!(p = get_free_proc_slot()))
		return NULL;
	
	set_proc(p, entry, priority, name);
	bitmap_fill(p->ptable, PTABLE_LEN);
	p->sp = alloc_stack(p);
	enqueue_tail(ready_q[priority], p);
	return p;
}


struct proc *start_user_proc(size_t *lines, size_t length, size_t entry, int priority, char *name){
	struct proc *p = NULL;
	if(p = get_free_proc_slot()) {
		p = exec_proc(p,lines,length,entry,priority,name);
	}
	return p;
}


void unseched(struct proc *p){
	release_proc_mem(p);
	remove_from_scheduling_queue(p);
}

void free_slot(struct proc *p){
	p->state = DEAD;
	p->IN_USE = 0;
	enqueue_tail(free_proc, p);
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
void end_process(struct proc *p) {
	int i,ret;
	unseched(p);
	free_slot(p);
}

//print out the list of processes currently in the ready_q
//and the currently running process
//return OK;

void process_overview() {
	int i;
	struct proc *curr;
	kprintf("NAME     proc_nr PPID RBASE      PC         STACK      HEAP       PROTECTION   flags\n");
	for (i = 0; i < NUM_PROCS; i++) {
		curr = &proc_table[i];
		if(curr->IN_USE && curr->state != ZOMBIE)
			printProceInfo(curr);
	}
}

//print the process state given
void printProceInfo(struct proc* curr) {
	int ptable_idx = get_page_index(curr->rbase)/32;
	kprintf("%-08s %-03d %-04d 0x%08x 0x%08x 0x%08x 0x%08x %d 0x%08x %d\n",
	        curr->name,
	        curr->proc_nr,
			curr->parent,
	        curr->rbase,
	        get_physical_addr(curr->pc,curr),
	        get_physical_addr(curr->sp,curr),
	        curr->heap_break,
			ptable_idx,
	        curr->ptable[ptable_idx],
			curr->flags);
}


/**
 * Gets a pointer to a process.
 *
 * Parameters:
 *   proc_nr		The process to retrieve.
 *
 * Returns:			The relevant process, or NULL if it does not exist.
 **/
struct proc *get_proc(int proc_nr) {
	struct proc *p;
	if (IS_PROCN_OK(proc_nr)){
		p = &proc_table[proc_nr];
		return p;
		// if(p->state != ZOMBIE && p->state != DEAD)
		// 	return p;
	}
	return NULL;
}

struct proc *get_running_proc(int proc_nr){
	struct proc *p = get_proc(proc_nr);
	if(p->state != RUNNABLE)
		return NULL;
	return p;
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
	struct proc *p;
	//Initialise queues

	for (i = 0; i < NUM_QUEUES; i++) {
		ready_q[i][HEAD] = NULL;
		ready_q[i][TAIL] = NULL;
	}

	free_proc[HEAD] = free_proc[TAIL] = NULL;
	//Add all proc structs to the free list
	for ( i = 0; i < NUM_PROCS; i++) {
		p = &proc_table[i];
		p->state = DEAD;
		p->IN_USE = 0;
		p->proc_nr = i;
		enqueue_tail(free_proc, p);
	}

	//No current process yet.
	current_proc = NULL;
}



