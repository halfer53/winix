/**
 * Userspace system call library for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#ifndef _SYSCALL_H_
#define _SYSCALL_H_ 1

#include <sys/ipc.h>

/**
 * System Call Numbers
 **/
#define SYSCALL_UPTIME		1
#define SYSCALL_EXIT		2
#define SYSCALL_FORK		3
#define SYSCALL_VFORK		4
#define SYSCALL_EXECVE		5
#define SYSCALL_BRK    		6
#define SYSCALL_ALARM       7
#define SYSCALL_SIGNAL		8
#define SYSCALL_SIGRET      9
#define SYSCALL_WAIT        10
#define SYSCALL_KILL        11
#define SYSCALL_GETPID      12
#define SYSCALL_WINFO		13
#define SYSCALL_GETC        14
#define SYSCALL_PRINTF    	15

#define SYSCALL_DEFAULT_RETURN  m.i1

#define DECLARE_SYSCALL(function, params, syscall_num, passing_codes)\
function params{\
    struct message m;\
    int __ret;\
    m.type = syscall_num;\
    passing_codes;\
    __ret = winix_sendrec(SYSTEM_TASK,&m);\
    if(m.i1 < 0){\
        __errno = -m.i1;\
        m.i1 = -1;\
    }else{\
        __errno = 0;\
    }\
    return m.i1;\
}\


#define WINFO_PS    1
#define WINFO_MEM   2
#define WINFO_SLAB  4

int _SYSCALL(int syscall_num, struct message *m);

/**
 * Returns the current system uptime, specified as the number of ticks since boot.
 **/
int sys_uptime();

/**
 * Exits the current process.
 **/
int _exit(int status);
int sys_ps();
pid_t fork();
int brk(void *addr);
void *sbrk(int incr);

int getc();
int printf(const char *format, ...);
unsigned int alarm(unsigned int seconds);
pid_t wait(int *wstatus);
pid_t getpid();
pid_t getppid();
int kill (pid_t pid, int sig);

#endif
