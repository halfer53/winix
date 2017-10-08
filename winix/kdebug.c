/**
 * 
 * Winix debug utilities
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:11:47
 * 
*/
#include <kernel/kernel.h>

PRIVATE int _debug_sched = 0;
PRIVATE int _debug_ipc = 0;
PRIVATE int _debug_syscall = 0;
PRIVATE int _debug_timer = 0;

/**
 * Testing functions for debugging purpoess
 * NB that debugging scheduling can be tricky because the screen
 * could be flood with scheduling information when the system runs, 
 * but we onlyl want to see how scheduling algorithms work at a given
 * period. Those counter is introduced for debugging scheduling and ipc.
 * Counter is decremented for each debugging information it displays
 */

int get_debug_sched_count(){
    if(_debug_sched > 0)
        return _debug_sched--;
    return 0;
}

int get_debug_ipc_count(){
    if(_debug_ipc > 0)
        return _debug_ipc--;
    return 0;
}

int get_debug_timer_count(){
    if(_debug_timer > 0)
        return _debug_timer--;
    return 0;
}

int is_debugging_syscall(){
    return _debug_syscall;
}

void debug_scheduling(int val){
    _debug_sched = val;
}

void debug_ipc(int val){
    _debug_ipc = val;
}

void debug_syscall(){
    _debug_syscall = true;
}

void debug_timer(int val){
    _debug_timer = val;
}

void stop_debug_scheduling(){
    _debug_sched = 0;
}

void stop_debug_ipc(){
    _debug_ipc = 0;
}

void stop_debug_syscall(){
    _debug_syscall = 0;
}




//print out the list of processes currently in the ready_q
//and the currently running process
void kprint_runnable_procs() {
    struct proc *curr;
    kprintf("NAME    PID PPID RBASE      PC         STACK      HEAP       PROTECTION   FLAGS\n");
    for_each_proc(curr){
        if(IS_RUNNABLE(curr)){
            kprint_proc_info(curr);
        }
    }
}

//print the process state
void kprint_proc_info(struct proc* curr) {
    int ptable_idx = PADDR_TO_PAGED(curr->rbase)/32;
    kprintf("%-08s %-03d %-04d 0x%08x 0x%08x 0x%08x 0x%08x %d 0x%08x %d %d\n",
            curr->name,
            curr->pid,
            get_proc(curr->parent)->pid,
            curr->rbase,
            get_physical_addr(get_pc_ptr(curr),curr),
            get_physical_addr(curr->sp,curr),
            curr->heap_break,
            ptable_idx,
            curr->ptable[ptable_idx],
            curr->s_flags,
            curr->proc_nr);
}

void kprint_readyqueue(){
    int i,j;
    struct proc* curr;
    kprintf(" q| ");
    for (i = 0; i < NUM_QUEUES; i++) {
        curr = ready_q[i][HEAD];
        while(curr != NULL)
        {
            kprintf("%d ", curr->proc_nr);
            curr = curr->next;
        }
    }
    kprintf("| ");
}

void kprint_receiver_queue(struct proc* who){
    struct proc* curr;
    curr = who->sender_q;
    if(curr)
        kprintf(" %d sending queue: ", who->proc_nr);
    while(curr != NULL){
        kprintf("%d ",curr->proc_nr);
        curr = curr->next_sender;
    }
}


