/**
 * Process Management for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#ifndef _W_PROC_H_
#define _W_PROC_H_

#include <winix/kwramp.h>
#include <signal.h>
#include <sys/ipc.h>
#include <winix/wini_ipc.h>

//Process & Scheduling
#define PROC_NAME_LEN			20
#define NUM_PROCS				20
#define NUM_QUEUES				5
#define IDLE_PRIORITY			4
#define USER_PRIORITY			3
#define KMOD_PRIORITY			2
#define SYSTEM_PRIORITY			1
#define TOP_PRIORITY			0

//Signal Context
#define PROCESS_CONTEXT_LEN		21

//Process Defaults
#define DEFAULT_FLAGS			0
#define PROTECTION_TABLE_LEN	32
#define DEFAULT_STACK_SIZE		1024
#define DEFAULT_HEAP_SIZE		1024
#define DEFAULT_CCTRL			0xff9
#define DEFAULT_STACK_POINTER			0x00000
#define USER_CCTRL			0x8 //OKU is set to 0
#define DEFAULT_RBASE			0x00000
#define DEFAULT_PTABLE			0x00000
#define DEFAULT_QUANTUM			100
#define DEFAULT_REG_VALUE		0xffffffff
#define DEFAULT_MEM_VALUE		0xffffffff
#define DEFAULT_RETURN_ADDR		0x00000000
#define DEFAULT_PROGRAM_COUNTER	0x00000000

//Process Flags
#define SENDING					0x0001
#define RECEIVING				0x0002
#define WAITING					0x0004

/**
 * State of a process.
 **/
typedef enum { DEAD, INITIALISING, RUNNABLE, ZOMBIE } proc_state_t;

/**
 * Process structure for use in the process table.
 *
 * Note: 	Do not update this structure without also
 * 			updating the definitions in "wramp.s"
 **/
typedef struct proc {
	/* Process State */
	unsigned long regs[NUM_REGS];	//Register values
	unsigned long *sp;
	void *ra;
	void (*pc)();
	void *rbase;
	unsigned long *ptable;
	unsigned long cctrl;  //len 19

	/* IPC */
	message_t *message;	//Message buffer;
	int flags; 

	/* Scheduling Priority */
	int priority;	
	int quantum;		//Timeslice length
	int ticks_left;		//Timeslice remaining
	//len 26

	/* Protection */
	unsigned long protection_table[PROTECTION_TABLE_LEN];

	/* IPC */
	struct proc *sender_q;	//Head of process queue waiting to send to this process
	struct proc *next_sender; //Link to next sender in the queue

	/* Scheduling */
	struct proc *next;	//Next pointer

	/* Accounting */
	int time_used;		//CPU time used

	/* Metadata */
	char name[PROC_NAME_LEN];		//Process name
	proc_state_t state;	//Current process state

	/* Process Table Index */
	int proc_index;		//Index in the process table

	unsigned long length;

	int parent_proc_index;
	int exit_status;

	void *heap_break;

	struct sigaction sig_table[_NSIG];
} proc_t;

extern proc_t proc_table[NUM_PROCS];
extern proc_t *ready_q[NUM_QUEUES][2];
extern proc_t *block_q[2];
void enqueue_tail(proc_t **q, proc_t *proc);
 void enqueue_head(proc_t **q, proc_t *proc);
 proc_t *dequeue(proc_t **q);
/**
 * Initialises the process table and scheduling queues.
 **/
void init_proc();
void proc_set_default(proc_t *p);
/**
 * Creates a new process and adds it to the runnable queue.
 **/
proc_t *new_proc(void (*entry)(), int priority, const char *name);
proc_t *get_free_proc();
void add_to_scheduling_queue(proc_t* p);
/**
 * WINIX Scheduler.
 **/
void sched();

/**
 * Frees up a process.
 *
 * Parameters:
 *   p		The process to remove.
 **/
void end_process(proc_t *p);

/**
 * Gets a pointer to a process.
 *
 * Parameters:
 *   proc_nr		The process to retrieve.
 *
 * Returns:			The relevant process, or NULL if it does not exist.
 **/
proc_t *get_proc(int proc_nr);


//fork the next process in the ready_q, return the new proc_index of the forked process
//side effect: the head of the free_proc is dequeued, and added to the ready_q with all relevant values equal
//to the original process, except stack pointer.
proc_t* do_fork(proc_t *p);
proc_t *kexecp(proc_t *p,void (*entry)(), int priority, const char *name);
proc_t *start_system(void (*entry)(), int priority, const char *name);
proc_t* start_init(size_t *lines, size_t length, size_t entry);


int process_overview();
void printProceInfo(proc_t* curr);
char* getStateName(proc_state_t state);
proc_t *pick_proc();


/**
 * Pointer to the current process.
 **/
extern proc_t *current_proc;

#endif
