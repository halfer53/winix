/**
 * 
 * Process Management for WINIX.
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

#ifndef _W_PROC_H_
#define _W_PROC_H_ 1

#include <winix/timer.h>
#include <winix/kwramp.h>

//Kernel Process
//Plz do make sure IDLE has the lowest process number
#define NUM_TASKS                   2   //number of kernel tasks,
#define IDLE                    	-1
#define SYSTEM                      0

//User Process
#define NUM_INIT_TASKS              1  //number of initial tasks, it's just init
#define INIT                   		1


//Process & Scheduling
#define PROC_NAME_LEN           	20
#define NUM_PROCS               	10
#define NUM_USER_PROCS              (NUM_PROCS - NUM_TASKS)
#define NUM_QUEUES              	5
#define MAX_PRIORITY            	0
#define MIN_PRIORITY            	((NUM_QUEUES) - 1)

//min bss segment size
#define MIN_BSS_SIZE            	200

//stack
#define STACK_MAGIC             	0x12345678
#define USER_STACK_SIZE         	1024
#define KERNEL_STACK_SIZE       	1024

//heap
#define USER_HEAP_SIZE          	2048

//Signal PCB Context
#define SIGNAL_CTX_LEN          	21

//Process Defaults
#define DEFAULT_FLAGS            	0
#define PTABLE_LEN               	32
#define DEFAULT_CCTRL            	0xff9
#define DEFAULT_STACK_POINTER    	0x00000
#define USER_CCTRL               	0x8 //OKU is set to 0
#define DEFAULT_RBASE            	0x00000
#define DEFAULT_PTABLE           	0x00000
#define DEFAULT_KERNEL_QUANTUM   	64
#define DEFAULT_USER_QUANTUM		8
#define DEFAULT_REG_VALUE        	0xffffffff
#define DEFAULT_MEM_VALUE        	0xffffffff
#define DEFAULT_RETURN_ADDR        	0x00000000
#define DEFAULT_PROGRAM_COUNTER    	0x00000000

//Process Scheduling Flags s_flags, process is runnable when s_flags == 0
#define SENDING                    	0x0001    /* process blocked trying to SEND */
#define RECEIVING                	0x0002    /* process blocked trying to RECEIVE */
#define WAITING                    	0x0004    /* process blocked wait(2) */
#define SIGNALED                	0x0008    /* set when new kernel signal arrives */
#define SIG_PENDING                	0x0010    /* unready while signal being processed */

//Process Information flags
#define IN_USE                    	0x0001      /* process slot is in use */
#define RUNNABLE                	0x0002      /* Running in the system */
#define ZOMBIE                    	0x0004      /* Zombie process */
#define STOPPED                    	0x0008      /* Stopped by signals */
#define BILLABLE                	0x0010      /* Set when user is invoking a system call */
#define DISABLE_FIRST_PAGE          0x0020      /* Set when the first page of the user address space is disabled */

//alloc_proc_mem flags
#define PROC_SET_SP                	1
#define PROC_SET_HEAP            	2

//proc_memctll flags
#define PROC_ACCESS                	1
#define PROC_NO_ACCESS            	0

/**
 * Process structure for use in the process table.
 *
 * Note:     Do not update this structure without also
 *             updating the definitions in "wramp.s"
 **/
typedef struct proc {
    /* Process State */
    reg_t regs[NUM_REGS];        	//Register values
    reg_t *sp;
    reg_t *ra;
    void (*pc)();
    reg_t *rbase;
    reg_t *ptable;
    reg_t cctrl;                  	//len 19 words

    /* IPC messages */
    int s_flags;                 	//schedling flags
    struct message* message;    	//Message Buffer
                                	//len 21 words
                                	//DO NOT MODIFY or CHANGE the order of the above
                                	//fields unless you know what you are doing

    /* Heap and Stack*/
    ptr_t* stack_top;             	//Stack_top is the physical address
    ptr_t* heap_break;             	//Heap_break is also the physical address of the curr
                                	//Brk, retrived by syscall brk(2)
    ptr_t* heap_bottom;         	//Bottom of the process image
    size_t length;                 	//Length is the total of text + data segment

    /* Protection */
    reg_t protection_table[PTABLE_LEN];

    /* IPC queue */
    struct proc *sender_q;        	//Head of process queue waiting to send to this process
    struct proc *next_sender;     	//Link to next sender in the queue

    /* Pending messages, used by winix_notify */
    unsigned int notify_pending;	//bitmap for masking list of pending messages by system proc

    /* Scheduling */
    struct proc *next;            	//Next pointer
    int priority;                	//Priority
    int quantum;                	//Timeslice length
    int ticks_left;                	//Timeslice remaining

    /* Accounting */
    clock_t time_used;            	//CPU time used
    clock_t sys_time_used;        	//system time used while the system is executing on behalf 
                                	//of this proc

    /* Metadata */
    char name[PROC_NAME_LEN];    	//Process name
    int exit_status;            	//Storage for status when process exits
    int sig_status;                	//Storage for siginal status when process exits
    pid_t pid;                    	//Process id
    pid_t procgrp;                	//Pid of the process group (used for signals)
    pid_t wpid;                    	//pid this process is waiting for
    int parent;                    	//proc_index of parent
    int i_flags;                	//information flags

    /* Process Table Index */
    int proc_nr;                	//Index in the process table

    /* Signal Information */
    sigset_t pending_sigs;
    struct sigaction sig_table[_NSIG];

    /* Alarm */
    struct timer alarm;
} proc_t;

/**
* Pointer to the current process.
**/
extern struct proc *current_proc;

extern struct proc *proc_table;
extern struct proc *ready_q[NUM_QUEUES][2];
extern struct proc *block_q[2];

#define IS_PROCN_OK(i)                  ((i)> -NUM_TASKS && (i) < NUM_PROCS)
#define IS_PRIORITY_OK(priority)        (0 <= (priority) && (priority) < NUM_QUEUES)
#define IS_KERNEL_PROC(p)               ((p)->rbase == NULL)
#define IS_USER_PROC(p)                 ((p)->rbase != NULL)
#define IS_IDLE(p)                      ((p)->proc_nr == IDLE)
#define IS_SYSTEM(p)                    ((p)->proc_nr == SYSTEM)
#define IS_RUNNABLE(p)                  (((p)->i_flags & (IN_USE | RUNNABLE)) == (IN_USE | RUNNABLE))

#define CHECK_STACK(p)                  (*((p)->stack_top) == STACK_MAGIC)
#define GET_DEF_STACK_SIZE(who)         (IS_USER_PROC(who) ? USER_STACK_SIZE : KERNEL_STACK_SIZE)
#define GET_HEAP_TOP(who)               ((who)->stack_top + GET_DEF_STACK_SIZE(who))

//This macro assumes idle has the lowest process number in the system
#define for_each_proc_except_idle(curr)\
for(curr = proc_table + IDLE + 1; curr < proc_table + NUM_PROCS - NUM_TASKS + 1; curr++)

//proc_table points at inex zero of the process table, so proc_table + 1 
//simply starts at init (init has process number 1)
#define for_each_user_proc(curr)\
for(curr = proc_table + 1; curr < proc_table + NUM_PROCS - NUM_TASKS + 1 ; curr++)

struct initial_frame{
    int operation;
    int dest;
    struct message *pm;
    struct message m;
    unsigned int syscall_code;
};


void* get_pc_ptr(struct proc* who);
void enqueue_tail(struct proc **q, struct proc *proc);
void enqueue_head(struct proc **q, struct proc *proc);
struct proc *dequeue(struct proc **q);
void init_proc();
void proc_set_default(struct proc *p);
reg_t* alloc_stack(struct proc *who);
void set_proc(struct proc *p, void (*entry)(), const char *name);
struct proc *start_kernel_proc(void (*entry)(), int proc_nr, const char *name,int quantum);
struct proc *start_user_proc(size_t *lines, size_t length, size_t entry, int priority, const char *name);
struct proc *get_free_proc_slot();
int alloc_proc_mem(struct proc *who, int tdb_length, int stack_size, int heap_size, int flags);
void enqueue_schedule(struct proc* p);
reg_t* alloc_kstack(struct proc *who);
int proc_memctl(struct proc* who ,vptr_t* page_addr, int flags);
void end_process(struct proc *p);
struct proc *get_proc(int proc_nr);
struct proc *get_running_proc(int proc_nr);
void kprint_runnable_procs();
void kprint_proc_info(struct proc* curr);
struct proc *pick_proc();
void unsched(struct proc *p);
int copyto_user_stack(struct proc *who, void *src, size_t len);
vptr_t* copyto_user_heap(struct proc* who, void *src, size_t len);
int build_initial_stack(struct proc* who, int argc, char** argv, char** env, struct proc* srcproc);

#endif
