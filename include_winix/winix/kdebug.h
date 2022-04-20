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

int is_debugging_ipc();
int is_debugging_sched();
int is_debugging_syscall();
void debug_scheduling(int val);
void debug_ipc(int val);
void debug_syscall(int val);
void stop_debug_scheduling();
void stop_debug_ipc();
void stop_debug_syscall();
void kprintf_syscall_reply(int reply);
void kprintf_syscall_request(int,pid_t);

#ifdef _DEBUG
    #define KDEBUG(token)   klog token
#else
    #define KDEBUG(token)
#endif

#endif
