#include <winix/kernel.h>

#define SCHED_DEBUG_LEN 30
#define IPC_DEBUG_LEN   10

int DEBUG_SCHED = 0;
int DEBUG_IPC = 0;

void start_sched_debug(){
    DEBUG_SCHED = SCHED_DEBUG_LEN;
}

void stop_sched_debug(){
    DEBUG_SCHED = 0;
}

void start_ipc_debug(){
    DEBUG_IPC = IPC_DEBUG_LEN;
}

void stop_ipc_debug(){
    DEBUG_IPC = 0;
}



