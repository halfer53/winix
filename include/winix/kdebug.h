/**
 * 
 * kernel debugging
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:04
 * 
*/
#ifndef _W_DEBUG_
#define _W_DEBUG_ 1

int get_debug_ipc_count();
int get_debug_sched_count();
int is_debugging_syscall();
void debug_scheduling(int val);
void debug_ipc(int val);
void debug_syscall();
void stop_debug_scheduling();
void stop_debug_ipc();
void stop_debug_syscall();

#ifdef _DEBUG
    #define KPRINT_DEBUG(token)   kinfo token
#else
    #define KPRINT_DEBUG(token)
#endif

#endif
