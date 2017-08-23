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

void debug_scheduling(int val){
    _debug_sched = val;
}

void debug_ipc(){
    _debug_ipc = INT_MAX;
}

void debug_syscall(){
    _debug_ipc = INT_MAX;
}

void stop_debug_scheduling(){
    _debug_sched = 0;
}

void stop_debug_ipc(){
    _debug_ipc = 0;
}



