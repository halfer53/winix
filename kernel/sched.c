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
#include <kernel/kernel.h>
#include <kernel/exception.h>
#include <kernel/sched.h>

PRIVATE struct timer sched_timer;

void init_sched(){
    memset(&sched_timer, 0, sizeof(struct timer));
    new_timer(SYSTEM_TASK, &sched_timer, REBALANCE_TIMEOUT, rebalance_queues);
}

/**
 * This method is called every REBALANCE_TIMEOUT timer interrupts
 * It effectly moves every processes in the ready queues
 * to the top priority ready queue, refer to Multi-fedback
 * queue scheduling for more details
 *  
 **/
void rebalance_queues(int proc_nr, clock_t time){
    struct proc* curr;
    foreach_proc(curr){
        curr->priority = DEFAULT_PRIORITY;
    }

    new_timer(SYSTEM_TASK, &sched_timer, REBALANCE_TIMEOUT, rebalance_queues);
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
    struct proc* mp;

    // Find the highest-priority non-empty queue
    for (i = MAX_PRIORITY; i >= MIN_PRIORITY; i--){
        if(mp = dequeue(ready_q[i])){
            return mp;
        }
    }

    PANIC("No procs left");
    return NULL;
}


/**
 * The Scheduler.
 *
 * Notes:
 *   Context of curr_scheduling_proc must already be saved.
 *   If successful, this function does not return.
 *
 * Side Effects:
 *   curr_scheduling_proc has its accounting fields updated, and is reinserted to ready_q.
 *   curr_scheduling_proc is updated to point to the next runnable process.
 *   Context of the new proc is loaded.
 **/
void sched() {
    int signum;
    // irq count is increased for each exception being called, and cleared on exiting
    // exception
    reset_irq_count();

    if (curr_scheduling_proc && !curr_scheduling_proc->state) {

        if (curr_scheduling_proc->ticks_left > 0) {
            enqueue_head(ready_q[curr_scheduling_proc->priority], curr_scheduling_proc);
        }
        else {
            // move the proc down to the lower ready queue, unless this proc
            // if already at the lowest ready queue, for every REBALANCE_TIMEOUT timer interrupts
            // rebalance_queue is called which bumps every processes in the top
            // ready queue
            if(IS_USER_PROC(curr_scheduling_proc) && curr_scheduling_proc->priority > MIN_PRIORITY ){
                curr_scheduling_proc->priority--;
            }
            enqueue_tail(ready_q[curr_scheduling_proc->priority], curr_scheduling_proc);
            
        }
    }

    do{
        curr_scheduling_proc = pick_proc();    
        signum = is_sigpending(curr_scheduling_proc);
        if(signum){
            handle_sig(curr_scheduling_proc, signum);
        }
            
    }while(curr_scheduling_proc == NULL || curr_scheduling_proc->state);
    
    if(is_debugging_sched()){
        kprintf2("|%d| ", curr_scheduling_proc->proc_nr);
    }
    // Reset quantum if needed
    if (curr_scheduling_proc->ticks_left <= 0) {
        curr_scheduling_proc->ticks_left = curr_scheduling_proc->quantum;
    }

    // Load context and run
    wramp_load_context();
}

