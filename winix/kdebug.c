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
    if(_debug_syscall > 0)
        return _debug_syscall--;
    return 0;
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

void kreport_readyqueue(){
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
    kprintf("| \n");
}

void kreport_receiver_queue(struct proc* who){
    struct proc* curr;
    curr = who->sender_q;
    if(curr)
        kprintf(" %d sending queue: ", who->proc_nr);
    while(curr != NULL){
        kprintf("%d ",curr->proc_nr);
        curr = curr->next_sender;
    }
}


