/**
 * Userspace system call library for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/**
 * System Call Numbers
 **/
#define SYSCALL_GETC		1
#define SYSCALL_UPTIME		2
#define SYSCALL_EXIT		3
#define SYSCALL_PS			4
#define SYSCALL_FORK		5
#define SYSCALL_VFORK		6
#define SYSCALL_EXEC		7
#define SYSCALL_SBRK    	8
#define SYSCALL_BRK    		9
#define SYSCALL_PUTC    	10
#define SYSCALL_PRINTF    	11
#define SYSCALL_LOAD_SREC	12
#define SYSCALL_CREATE		13
#define SYSCALL_OPEN		14
#define SYSCALL_READ		15
#define SYSCALL_WRITE		16
#define SYSCALL_CLOSE		17
#define SYSCALL_ALARM       18
#define SYSCALL_SIGNAL		19
#define SYSCALL_SIGRET      20
#define SYSCALL_WAIT        21
#define SYSCALL_KILL        22
#define SYSCALL_GETPID      23

#define PRINT_TOKEN(token) printf(#token " is %d", token)

#define SYSCALL_DEFAULT_RETURN  m.i1
#define DECLARE_SYSCALL(function, params, syscall_num, passing_codes, ipc, return_m)\
function params{\
    message_t m;\
    int __ret;\
    m.type = syscall_num;\
    passing_codes;\
    __ret = winix_sendrec(SYSTEM_TASK,&m);\
    return return_m;\
}\


// #define DECLARE_SYSCALL(name, parameters, passing_codes,return_codes)\       
//     name(parameters){\
//         message_t __m;
//         int __response = 0;
//         passing_codes
//         __response = winix_send(SYSTEM_TASK,&__m);
//         return_codes
//     }\

//TODO: create a sensible allocation scheme for system call numbers

/**
 * Returns the current system uptime, specified as the number of ticks since boot.
 **/
int sys_uptime();

/**
 * Exits the current process.
 **/
int sys_exit(int status);
int sys_process_overview();

void *sbrk(unsigned long size);

int getc();
int putc(int i);
int printf(const char *format, ...);

#endif
