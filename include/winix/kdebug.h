#ifndef _W_DEBUG_
#define _W_DEBUG_ 1

int get_debug_ipc_count();
int get_debug_sched_count();
void debug_scheduling(int val);
void debug_ipc();
void stop_debug_scheduling();
void stop_debug_ipc();

#ifdef _DEBUG
    #define KDEBUG(token)   kinfo token
#else
    #define KDEBUG(token)
#endif

#endif
