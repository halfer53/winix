/**
 * WINIX System Calls.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include <sys/ipc.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <ucontext.h>
#include <curses.h>
#include <signal.h>
#include <debug.h>

/**
 * Get the system uptime.
 **/
int sys_uptime() {
	int response = 0;
	message_t m;

	m.type = SYSCALL_UPTIME;
	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
	return m.i1;
}

/**
 * Exits the current process.
 **/
int sys_exit(int status) {
	int response = 0;
	message_t m;

	m.type = SYSCALL_EXIT;
	m.i1 = status;
	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
	return m.i1;
}

int sys_process_overview(){
	int response = 0;
	message_t m;

	m.type = SYSCALL_PS;
	response = winix_send(SYSTEM_TASK, &m); //TODO: error checking
	return OK;
}

pid_t fork(){
	int response = 0;
	message_t m;

	m.type = SYSCALL_FORK;
	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
	return (pid_t)m.i1;
}


int exec(){
	int response = 0;
	message_t m;

	m.type = SYSCALL_EXEC;
	response = winix_send(SYSTEM_TASK, &m); //TODO: error checking
	return OK;
}

void *sbrk(size_t size){
	int response = 0;
	message_t m;

	m.type = SYSCALL_SBRK;
	m.i1 = size;
	response = winix_sendrec(SYSTEM_TASK, &m);
	return m.p1;
}


int brk(void *addr){
	int response = 0;
	message_t m;

	m.type = SYSCALL_BRK;
	m.p1 = addr;
	response = winix_sendrec(SYSTEM_TASK, &m);
	return m.i1;
}


int getc(){
	int response = 0;
	message_t m;

	m.type = SYSCALL_GETC;
	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
	return m.i1;
}

void putc(int i){
	int response = 0;
	message_t m;

	m.type = SYSCALL_PUTC;
	//printf("putc syscall id %d, val %c\n",m.type,(char)i );
	m.i1 = i;
	response = winix_send(SYSTEM_TASK, &m); //TODO: error checking
	return;
}

int printf(const char *format, ...) {
	int response = 0;
	message_t m;
	
	m.type = SYSCALL_PRINTF;
	m.p1 = (void *)format;
	m.p2 = (void *)((int *)&format+1);
	response = winix_send(SYSTEM_TASK,&m);
	return OK;
}

sighandler_t signal(int signum, sighandler_t handler){
	int response = 0;
	message_t m;
	
	m.type = SYSCALL_SIGNAL;
	m.i1 = signum;
	m.s1 = handler;
	response = winix_send(SYSTEM_TASK,&m);
	return SIG_DFL;
}

void sigreturn(int signum){
	int addr;
	int response = 0;
	message_t m;

	m.type = SYSCALL_SIGRET;
	m.i1 = signum;
	response = winix_send(SYSTEM_TASK,&m);
}

unsigned long alarm(unsigned long seconds){
	int response = 0;
	message_t m;
	
	m.type = SYSCALL_ALARM;
	m.i1 = seconds;
	response = winix_send(SYSTEM_TASK,&m);
	return OK;
}

pid_t wait(int *wstatus){
	int response = 0;
	message_t m;
	
	m.type = SYSCALL_WAIT;
	m.p1 = wstatus;
	response = winix_sendrec(SYSTEM_TASK,&m);
	return (pid_t)m.i1;
}

pid_t getpid(){
	int response = 0;
	message_t m;
	
	m.type = SYSCALL_GETPID;
	response = winix_sendrec(SYSTEM_TASK,&m);
	return (pid_t)m.i1;
}