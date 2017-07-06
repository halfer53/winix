/**
 * WINIX System Calls.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include <lib.h>
#include <ucontext.h>
#include <curses.h>

#include <debug.h>

/**
 * Get the system uptime.
 **/
// int sys_uptime() {
// 	int response = 0;
// 	message_t m;

// 	m.type = SYSCALL_UPTIME;
// 	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
// 	return m.i1;
// }

DECLARE_SYSCALL(int sys_uptime, (), 
	SYSCALL_UPTIME,
	,
	sendrec,
	SYSCALL_DEFAULT_RETURN)

/**
 * Exits the current process.
 **/
// int sys_exit(int status) {
// 	int response = 0;
// 	message_t m;

// 	m.type = SYSCALL_EXIT;
// 	m.i1 = status;
// 	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
// 	return m.i1;
// }

DECLARE_SYSCALL(int sys_exit, (int status), 
	SYSCALL_EXIT,
	m.i1 = status,
	sendrec,
	SYSCALL_DEFAULT_RETURN)

// int sys_process_overview(){
// 	int response = 0;
// 	message_t m;

// 	m.type = SYSCALL_PS;
// 	response = winix_send(SYSTEM_TASK, &m); //TODO: error checking
// 	return m.i1;
// }

DECLARE_SYSCALL(int sys_process_overview, (), 
	SYSCALL_PS,
	,
	sendrec,
	SYSCALL_DEFAULT_RETURN)

// pid_t fork(){
// 	int response = 0;
// 	message_t m;

// 	m.type = SYSCALL_FORK;
// 	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
// 	return (pid_t)m.i1;
// }
DECLARE_SYSCALL(pid_t fork, (), 
	SYSCALL_FORK,
	,
	sendrec,
	(pid_t)m.i1)

// int exec(){
// 	int response = 0;
// 	message_t m;

// 	m.type = SYSCALL_EXEC;
// 	response = winix_send(SYSTEM_TASK, &m); //TODO: error checking
// 	return m.i1;
// }
DECLARE_SYSCALL(int exec, (), 
	SYSCALL_EXEC,
	,
	sendrec,
	SYSCALL_DEFAULT_RETURN)

// void *sbrk(size_t size){
// 	int response = 0;
// 	message_t m;

// 	m.type = SYSCALL_SBRK;
// 	m.i1 = size;
// 	response = winix_sendrec(SYSTEM_TASK, &m);
// 	return m.p1;
// }
DECLARE_SYSCALL(void *sbrk, (size_t size), 
	SYSCALL_SBRK,
	m.i1 = size,
	sendrec,
	m.p1)


// int brk(void *addr){
// 	int response = 0;
// 	message_t m;

// 	m.type = SYSCALL_BRK;
// 	m.p1 = addr;
// 	response = winix_sendrec(SYSTEM_TASK, &m);
// 	return m.i1;
// }
DECLARE_SYSCALL(int brk, (void *addr), 
	SYSCALL_BRK,
	m.p1 = addr,
	sendrec,
	SYSCALL_DEFAULT_RETURN)


// int getc(){
// 	int response = 0;
// 	message_t m;

// 	m.type = SYSCALL_GETC;
// 	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
// 	return m.i1;
// }
DECLARE_SYSCALL(int getc, (), 
	SYSCALL_GETC,
	,
	sendrec,
	SYSCALL_DEFAULT_RETURN)

// void putc(int i){
// 	int response = 0;
// 	message_t m;

// 	m.type = SYSCALL_PUTC;
// 	//printf("putc syscall id %d, val %c\n",m.type,(char)i );
// 	m.i1 = i;
// 	response = winix_send(SYSTEM_TASK, &m); //TODO: error checking
// 	return;
// }

DECLARE_SYSCALL(int putc, (int i), 
	SYSCALL_PUTC,
	m.i1 = i,
	sendrec,
	SYSCALL_DEFAULT_RETURN)

// int printf(const char *format, ...) {
// 	int response = 0;
// 	message_t m;
	
// 	m.type = SYSCALL_PRINTF;
// 	m.p1 = (void *)format;
// 	m.p2 = (void *)((int *)&format+1);
// 	response = winix_send(SYSTEM_TASK,&m);
// 	return m.i1;
// }

DECLARE_SYSCALL(int printf, (const char *format, ...), 
	SYSCALL_PRINTF,
	m.p1 = (void *)format; m.p2 = (void *)((int *)&format+1);,
	sendrec,
	SYSCALL_DEFAULT_RETURN)

// sighandler_t signal(int signum, sighandler_t handler){
// 	int response = 0;
// 	message_t m;
	
// 	m.type = SYSCALL_SIGNAL;
// 	m.i1 = signum;
// 	m.s1 = handler;
// 	response = winix_send(SYSTEM_TASK,&m);
// 	return SIG_DFL;
// }
DECLARE_SYSCALL(sighandler_t signal, (int signum, sighandler_t handler), 
	SYSCALL_SIGNAL,
	m.i1 = signum; m.s1 = handler,
	sendrec,
	SIG_DFL )

// void sigreturn(int signum){
// 	int addr;
// 	int response = 0;
// 	message_t m;

// 	m.type = SYSCALL_SIGRET;
// 	m.i1 = signum;
// 	response = winix_send(SYSTEM_TASK,&m);
// }
DECLARE_SYSCALL(int sigreturn, (int signum), 
	SYSCALL_SIGRET, 
	m.i1 = signum, 
	send,
	SYSCALL_DEFAULT_RETURN)

// unsigned int alarm(unsigned int seconds){
// 	int response = 0;
// 	message_t m;
	
// 	m.type = SYSCALL_ALARM;
// 	m.i1 = seconds;
// 	response = winix_send(SYSTEM_TASK,&m);
// 	return m.i1;
// }

DECLARE_SYSCALL(unsigned int alarm, (unsigned int seconds), 
	SYSCALL_ALARM, 
	m.i1 = seconds, 
	sendrec,
	SYSCALL_DEFAULT_RETURN)

// pid_t wait(int *wstatus){
// 	int response = 0;
// 	message_t m;
	
// 	m.type = SYSCALL_WAIT;
// 	m.p1 = wstatus;
// 	response = winix_sendrec(SYSTEM_TASK,&m);
// 	return (pid_t)m.i1;
// }
DECLARE_SYSCALL(pid_t wait, (int *wstatus), 
	SYSCALL_WAIT, 
	m.p1 = wstatus, 
	sendrec,
	(pid_t)m.i1)

// pid_t getpid(){
// 	int response = 0;
// 	message_t m;
	
// 	m.type = SYSCALL_GETPID;
// 	response = winix_sendrec(SYSTEM_TASK,&m);
// 	return (pid_t)m.i1;
// }
DECLARE_SYSCALL(pid_t getpid, (), 
	SYSCALL_GETPID, 
	, 
	sendrec,
	(pid_t)m.i1)

DECLARE_SYSCALL(int kill, (pid_t pid, int sig), 
	SYSCALL_KILL, 
	m.i1 = pid; m.i2 = sig , 
	sendrec,
	SYSCALL_DEFAULT_RETURN)



