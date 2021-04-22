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

#include <sys/ucontext.h>
#include <sys/types.h>
#include <signal.h>
#include <winix/type.h>
#include <winix/comm.h>
#include <winix/timer.h>
#include <winix/kwramp.h>
#include <fs/type.h>
#include <fs/inode.h>
#include <fs/filp.h>
#include <winix/welf.h>
#include <stdbool.h>

// Init
#define INIT                   		1
#define INTERRUPT                   -30

// Kernel Process
// Plz do make sure IDLE has the lowest process number
#define NUM_TASKS                   3
#define IDLE                    	-2
#define PARALLEL                    -1
#define SYSTEM                      0

// Number of User procs
#define NUM_PROCS               	8

// Total number of procs
#define NUM_PROCS_AND_TASKS         (NUM_TASKS + NUM_PROCS)

// Scheduling
#define NUM_QUEUES              	5
#define MAX_PRIORITY            	4
#define MIN_PRIORITY            	0
#define DEFAULT_PRIORITY            3

// Max string len for a process name 
//(including NULL terminator)
#define PROC_NAME_LEN           	16
#define PROC_FILEP_NR               16

// min bss segment size
#define MIN_BSS_SIZE            	200

// stack
#define STACK_MAGIC             	0x12345678
#define USER_STACK_SIZE         	PAGE_LEN
#define KERNEL_STACK_SIZE       	PAGE_LEN

// heap
#define USER_HEAP_SIZE          	PAGE_LEN

// Signal PCB Context
#define SIGNAL_CTX_LEN          	21

// Process Defaults
#define DEFAULT_FLAGS            	0
#define PTABLE_LEN               	32
#define DEFAULT_CCTRL            	0xff9
#define DEFAULT_STACK_POINTER    	0x00000
#define USER_CCTRL               	0x8 // OKU is set to 0
#define DEFAULT_RBASE            	0x00000
#define DEFAULT_PTABLE           	0x00000
#define DEFAULT_KERNEL_QUANTUM   	64
#define DEFAULT_USER_QUANTUM		8
#define DEFAULT_REG_VALUE        	0xffffffff
#define DEFAULT_MEM_VALUE        	0xffffffff
#define DEFAULT_RETURN_ADDR        	0x00000000
#define DEFAULT_PROGRAM_COUNTER    	0x00000000

// Process Scheduling Flags state, process is runnable when state == 0
#define STATE_RUNNABLE              0x00000000    /* Process is running or in the ready queue */
#define STATE_SENDING               0x00000001    /* process blocked trying to SEND */
#define STATE_RECEIVING             0x00000002    /* process blocked trying to RECEIVE */
#define STATE_WAITING               0x00000004    /* process blocked wait(2) */
#define STATE_PAUSING               0x00000008    /* process blocked by sigsuspend(2) or pause(2) */
#define STATE_VFORKING              0x00000010    /* parent is blocked by vfork(2) */
#define STATE_STOPPED               0x00000020    /* Stopped by SIGSTOP or SIGTSTP */
#define STATE_KILLED                0x00000040    /* Process killed by signal */  
#define STATE_ZOMBIE                0x80000000    /* Zombie process */

// Process Information flags
#define IN_USE                    	0x0001      /* process slot is in use */
#define BILLABLE                	0x0002      /* Set when user is invoking a system call */
#define DIRECT_SYSCALL              0x0004      /* Direct Syscall mode */

// proc_memctll flags
#define PROC_ACCESS                	1
#define PROC_NO_ACCESS            	0


struct k_context{
    mcontext_t m;
    reg_t *rbase;
    reg_t *ptable;
    reg_t cctrl;                  	// len 19 words
};

/**
 * Process structure for use in the process table.
 *
 * Note:     Do not update this structure without also
 *             updating the definitions in "wramp.s"
 **/
typedef struct proc {
    struct k_context ctx;

    /* IPC messages */
    int state;                      // schedling flags
    struct message* message;    	// Message Buffer
                                	// len 21 words
                                	// DO NOT MODIFY or CHANGE the order of the above
                                    // fields unless you know what you are doing
    

    /* Heap and Stack*/
    ptr_t* mem_start;
    ptr_t* stack_top;
    ptr_t* heap_top;                // 
    ptr_t* heap_break;             	// Heap_break is also the physical address of the curr
    ptr_t* heap_bottom;         	// Bottom of the process image

    size_t rbase_offset;
    size_t text_size;
    size_t data_size;
    size_t bss_size;

    /* Protection */
    reg_t protection_table[PTABLE_LEN];

    /* IPC queue */
    struct proc *sender_q;        	// Head of process queue waiting to send to this process
    struct proc *next_sender;     	// Link to next sender in the queue

    /* Pending messages, used by winix_notify */
    unsigned int notify_pending;	// bitmap for masking list of pending messages by system proc
    // struct list_head notify_queue;

    /* Scheduling */
    struct proc *next;            	// Next pointer
    int priority;                	// Priority
    int quantum;                	// Timeslice length
    int ticks_left;                	// Timeslice remaining

    /* Accounting */
    clock_t time_used;            	// CPU time used
    clock_t sys_time_used;        	// system time used while the system is executing on behalf 
                                	// of this proc

    /* Metadata */
    char name[PROC_NAME_LEN];    	// Process name
    uid_t uid;
    gid_t gid;
    int exit_status;            	// Storage for status when process exits
    int sig_status;                	// Storage for siginal status when process exits
    pid_t pid;                    	// Process id
    pid_t procgrp;                	// Pid of the process group (used for signals)
    pid_t session_id;               // Session id
    pid_t wpid;                    	// pid this process is waiting for
    vptr_t* varg;                   // virtual arg of system call, usually saved from int* status in waitpid(2), 
                                    // so that kernel can later return the proper value
    int woptions;                   // waiting options
    int parent;                    	// proc_index of parent
    int thread_parent;              // proc_index of parent whom share the memory except stack
    int flags;                	    // information flags
    clock_t syscall_start_time;

    /* Process Table Index */
    int proc_nr;                	// Index in the process table

    /* Signal Information */
    sigset_t sig_pending;
    sigset_t sig_mask;
    sigset_t sig_mask2;
    struct sigaction sig_table[_NSIG];
    reg_t* sa_restorer;

    /* Alarm */
    struct timer alarm;

    /* File System */
    mode_t umask;
    filp_t* fp_filp[OPEN_MAX];
    inode_t *fp_rootdir;
    inode_t *fp_workdir;
    struct list_head pipe_reading_list;
    struct list_head pipe_writing_list;

} proc_t;

/**
* Pointer to the current process.
**/
extern struct proc *curr_scheduling_proc;
extern struct proc *curr_syscall_caller;

extern struct proc *proc_table;
extern struct proc *ready_q[NUM_QUEUES][2];
extern struct proc *block_q[2];

#define SYSTEM_TASK                     (proc_table)

#define IS_PROCN_OK(i)                  ((i)> -NUM_TASKS && (i) <= NUM_PROCS)
#define IS_PRIORITY_OK(priority)        (0 <= (priority) && (priority) < NUM_QUEUES)
#define IS_KERNEL_PROC(p)               ((p)->ctx.rbase == NULL)
#define IS_KERNELN(n)                   ((n)<= 0 && (n)> -NUM_TASKS)
#define IS_USER_PROC(p)                 ((p)->ctx.rbase != NULL)

#define IS_IDLE(p)                      ((p)->proc_nr == IDLE)
#define IS_SYSTEM(p)                    ((p)->proc_nr == SYSTEM)

#define IS_INUSE(p)                     ((p)->flags & IN_USE)
#define IS_RUNNABLE(p)                  ((p)->state == STATE_RUNNABLE)
#define IS_ZOMBIE(p)                    (IS_INUSE(p) && (p)->state & STATE_ZOMBIE)
#define IS_BLOCKED(p)                   (IS_INUSE(p) && (p)->state > 0)

#define USER_ERRNO(p)                   (p->stack_top + 1)
#define USER_TMP_MESSAGE(p)             ((struct message*)p->stack_top + 2)

#define TASK_NR_TO_SID(tnr)             (tnr <= 0 ? -tnr + 1 : tnr)
#define SID_TO_TASK_NR(sid)             (-sid + 1)
#define SET_CALLER(pcurr)   (curr_syscall_caller = pcurr)


// proc_table points at index zero of the process table, so proc_table + INIT
// simply starts at init
#define foreach_proc(curr)\
for(curr = proc_table + INIT; curr <= proc_table + NUM_PROCS ; curr++)\
    if(IS_INUSE(curr))

#define foreach_ktask(curr)\
    for(curr = proc_table - NUM_TASKS + 1 ; curr <= proc_table ; curr++)\

#define foreach_proc_and_task(curr)\
for(curr = proc_table - NUM_TASKS + 1; curr <= proc_table + NUM_PROCS; curr++)\
    if(IS_INUSE(curr))

#define foreach_blocked_proc(curr)\
for(curr = proc_table + 1; curr <= proc_table + NUM_PROCS; curr++)\
    if(IS_INUSE(curr) && curr->state > 0)

#define foreach_child(curr, parent_proc)\
for(curr = proc_table + 1; curr <= proc_table + NUM_PROCS; curr++)\
    if(IS_INUSE(curr) && (curr)->parent == (parent_proc)->proc_nr)


void* get_pc_ptr(struct proc* who);
void enqueue_tail(struct proc **q, struct proc *proc);
void enqueue_head(struct proc **q, struct proc *proc);
struct proc *dequeue(struct proc **q);
void init_proc();
void proc_set_default(struct proc *p);
reg_t* alloc_stack(struct proc *who);
void set_proc(struct proc *p, void (*entry)(), const char *name);
struct proc *start_user_proc(size_t *lines, size_t length, size_t entry, int priority, const char *name);
struct proc *get_free_proc_slot();
int alloc_proc_mem(struct proc *who, int tdb_length, int stack_size, int heap_size);
void enqueue_schedule(struct proc* p);
reg_t* alloc_kstack(struct proc *who, int size);
int proc_memctl(struct proc* who ,vptr_t* page_addr, int flags);
pid_t get_next_pid();
struct proc* get_proc_by_pid(pid_t pid);
struct proc *get_proc(int proc_nr);
struct proc *get_runnable_proc(int proc_nr);
void kreport_all_procs(struct filp*);
void kreport_proc(struct proc* curr, struct filp* );
struct proc *pick_proc();
void zombify(struct proc *p);
void release_zombie(struct proc*p);
int copyto_user_stack(struct proc *who, void *src, size_t len);
vptr_t* copyto_user_heap(struct proc* who, void *src, size_t len);
int build_initial_stack(struct proc* who, char** argv, char** env, struct proc* srcproc);
int exec_welf(struct proc* who, char* path, char *argv[], char *envp[], bool is_new);
void release_proc_mem(struct proc *who);
int alloc_mem_welf(struct proc* who, struct winix_elf* elf, int stack_size, int heap_size);
void task_exit();
void wramp_break();

#define release_proc_slot(p)    release_zombie(p)

#endif


