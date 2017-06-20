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
#define SYSCALL_EXEC		6
#define SYSCALL_SBRK    	7
#define SYSCALL_BRK    		8
#define SYSCALL_PUTC    	9
#define SYSCALL_PRINTF    	10
#define SYSCALL_LOAD_SREC	11
#define SYSCALL_CREATE		12
#define SYSCALL_OPEN		13
#define SYSCALL_READ		14
#define SYSCALL_WRITE		15
#define SYSCALL_CLOSE		16
#define SYSCALL_SIGNAL		17
#define SYSCALL_SIGRET      18

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
void putc(int i);
int printf(const char *format, ...);

#endif
