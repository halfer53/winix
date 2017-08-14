/**
 * Userspace system call library for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#ifndef _SYSCALL_H_
#define _SYSCALL_H_ 1

/**
 * System Call Numbers
 **/
#define SYSCALL_UPTIME		1
#define SYSCALL_EXIT		2
#define SYSCALL_FORK		3
#define SYSCALL_VFORK		4
#define SYSCALL_EXECVE		5
#define SYSCALL_SBRK    	6
#define SYSCALL_BRK    		7
#define SYSCALL_ALARM       8
#define SYSCALL_SIGNAL		9
#define SYSCALL_SIGRET      10
#define SYSCALL_WAIT        11
#define SYSCALL_KILL        12
#define SYSCALL_GETPID      13
#define SYSCALL_GETPPID     14
#define SYSCALL_GETC        15
#define SYSCALL_PS			16
#define SYSCALL_PRINTF    	17

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

/**
 * Returns the current system uptime, specified as the number of ticks since boot.
 **/
int sys_uptime();

/**
 * Exits the current process.
 **/
int _exit(int status);
int sys_ps();

void *sbrk(unsigned int size);

int getc();
int printf(const char *format, ...);

#endif
