#include <kernel/kernel.h>

PRIVATE int _debug_sched = 0;
PRIVATE int _debug_ipc = 0;

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



