/**
 * Process Management for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#ifndef _W_PROC_H_
#define _W_PROC_H_ 1

#include <winix/timer.h>
#include <winix/kwramp.h>

//Process & Scheduling
#define PROC_NAME_LEN			20
#define NUM_PROCS				13
#define NUM_QUEUES				5
#define IDLE_PRIORITY			4
#define USER_PRIORITY			3
#define SYSTEM_PRIORITY			0

#define USER_PRIORITY_MAX		1
#define USER_PRIORITY_MIN		4

//Signal Context
#define PROCESS_REGS_LEN		16
#define SIGNAL_CTX_LEN			21

//Process Defaults
#define DEFAULT_FLAGS			0
#define PTABLE_LEN				32
#define DEFAULT_CCTRL			0xff9
#define DEFAULT_STACK_POINTER	0x00000
#define USER_CCTRL				0x8 //OKU is set to 0
#define DEFAULT_RBASE			0x00000
#define DEFAULT_PTABLE			0x00000
#define DEFAULT_KERNEL_QUANTUM	64
#define DEFAULT_USER_QUANTUM	2
#define DEFAULT_REG_VALUE		0xffffffff
#define DEFAULT_MEM_VALUE		0xffffffff
#define DEFAULT_RETURN_ADDR		0x00000000
#define DEFAULT_PROGRAM_COUNTER	0x00000000

//stack
#define STACK_MAGIC				0x12345678
#define USER_STACK_SIZE		1024
#define KERNEL_STACK_SIZE		2048

//heap
#define USER_HEAP_SIZE		2048

//Process Flags
#define SENDING					0x0001
#define RECEIVING				0x0002
#define WAITING					0x0004

//alloc_proc_mem flags
#define PROC_SET_SP				1
#define PROC_SET_HEAP			2

//proc_memctll flags
#define PROC_ACCESS				1
#define PROC_NO_ACCESS			0

//min bss segment size
#define MIN_BSS_SIZE			300

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
	reg_t regs[NUM_REGS];	//Register values
	reg_t *sp;
	reg_t *ra;
	void (*pc)();
	reg_t *rbase;
	reg_t *ptable;
	reg_t cctrl;  //len 19

	/* IPC messages */
	struct message *message;	//Message buffer;
	int flags; 

	/* Heap and Stack*/
	ptr_t* stack_top; //stack_top is the physical address
	ptr_t* heap_break; //heap_break is also the physical address
	ptr_t* heap_bottom; //
	size_t length; //length is depreciated, do not use it

	/* Protection */
	reg_t protection_table[PTABLE_LEN];

	/* IPC queue */
	struct proc *sender_q;	//Head of process queue waiting to send to this process
	struct proc *next_sender; //Link to next sender in the queue

	/* Scheduling */
	struct proc *next;	//Next pointer
	int priority;	
	int quantum;		//Timeslice length
	int ticks_left;		//Timeslice remaining

	/* Accounting */
	int time_used;		//CPU time used

	/* Metadata */
	char name[PROC_NAME_LEN];		//Process name
	proc_state_t state;	//Current process state
	int exit_status;	//Storage for status when process exits
	int sig_status;		//Storage for siginal status when process exits
	pid_t pid;			//Process id
	pid_t procgrp;		//Pid of the process group (used for signals)
	pid_t wpid;			//pid this process is waiting for
	int parent;			//proc_index of parent

	/* Process Table Index */
	int proc_nr;		//Index in the process table
	int IN_USE;			//Whether the current slot is in use

	/* Signal Information */
	sigset_t pending_sigs;
	struct sigaction sig_table[_NSIG];

	/* Alarm */
	struct timer alarm;
} proc_t;


#define IS_PROCN_OK(i)	((i)>= 0 && (i) < NUM_PROCS)
#define IS_PRIORITY_OK(priority)	(0 <= (priority) && (priority) < NUM_QUEUES)
#define IS_KERNEL_PROC(p)	((p->proc_nr) == 0)
#define IS_USER_PROC(p)		((p->proc_nr) > 0)
#define CHECK_STACK(p)		(*(p->stack_top) == STACK_MAGIC)
#define GET_DEF_STACK_SIZE(who)	(IS_USER_PROC(who) ? USER_STACK_SIZE : KERNEL_STACK_SIZE)
#define GET_HEAP_TOP(who)	((who)->stack_top + GET_DEF_STACK_SIZE(who))


extern struct proc proc_table[NUM_PROCS];
extern struct proc *ready_q[NUM_QUEUES][2];
extern struct proc *block_q[2];


void enqueue_tail(struct proc **q, struct proc *proc);
void enqueue_head(struct proc **q, struct proc *proc);
struct proc *dequeue(struct proc **q);
void init_proc();
void proc_set_default(struct proc *p);
reg_t* alloc_stack(struct proc *who);
int set_proc(struct proc *p, void (*entry)(), int priority, const char *name);
struct proc *start_kernel_proc(void (*entry)(), int priority, const char *name);
struct proc *start_user_proc(size_t *lines, size_t length, size_t entry, int priority, const char *name);
struct proc *get_free_proc_slot();
int alloc_proc_mem(struct proc *who, int tdb_length, int stack_size, int heap_size, int flags);
void enqueue_schedule(struct proc* p);
reg_t* alloc_kstack(struct proc *who);
int proc_memctl(struct proc* who ,vptr_t* page_addr, int flags);
/**
 * WINIX Scheduler.
 **/
void sched();
void end_process(struct proc *p);
struct proc *get_proc(int proc_nr);
struct proc *get_running_proc(int proc_nr);


//fork the next process in the ready_q, return the new pid of the forked process
//side effect: the head of the free_proc is dequeued, and added to the ready_q with all relevant values equal
//to the original process, except stack pointer.
void process_overview();
void printProceInfo(struct proc* curr);
char* getStateName(proc_state_t state);
struct proc *pick_proc();


/**
 * Pointer to the current process.
 **/
extern struct proc *current_proc;


#endif
