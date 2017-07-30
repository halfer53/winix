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

static pid_t __pid_cache = 0;//pid cache

int _SYSCALL(int syscall_num, message_t *m){
	m->type = syscall_num;
	winix_sendrec(SYSTEM_TASK, m); 
	if(m->i1 < 0){
		__set_errno(-m->i1);
		return -1;
	}
	return m->i1;
}

/**
 * Get the system uptime.
 **/
int sys_uptime() {
	message_t m;
	return _SYSCALL(SYSCALL_UPTIME,&m);
}

/**
 * Exits the current process.
 **/
int _exit(int status) {
	message_t m;
	m.i1 = status;
	return _SYSCALL(SYSCALL_EXIT,&m);
}

int sys_ps(){
	message_t m;
	return _SYSCALL(SYSCALL_PS,&m);
}

pid_t fork(){
	int result;
	message_t m;
	result = _SYSCALL(SYSCALL_FORK,&m);
	if(result == 0)
		__pid_cache = 0; //reset pid cache
	return result;
}

void *sbrk(size_t size){
	message_t m;
	m.i1 = size;
	_SYSCALL(SYSCALL_SBRK,&m);
	return m.p1;
}

int brk(void *addr){
	message_t m;
	m.p1 = addr;
	return _SYSCALL(SYSCALL_BRK,&m);
}

int exec(){
	message_t m;
	return _SYSCALL(SYSCALL_EXECVE,&m);
}


int getc(){
	message_t m;
	return _SYSCALL(SYSCALL_GETC,&m);
}

int putc(int i){
	message_t m;
	m.i1 = i;
	return _SYSCALL(SYSCALL_PUTC,&m);
}

int printf(const char *format, ...) {
	message_t m;
	m.p1 = (void *)format;
	m.p2 = (void *)((int *)&format+1);
	return _SYSCALL(SYSCALL_PRINTF,&m);
}


sighandler_t signal(int signum, sighandler_t handler){
	message_t m;
	m.i1 = signum;
	m.s1 = handler;
	_SYSCALL(SYSCALL_SIGNAL,&m);
	return m.s1;
}

unsigned int alarm(unsigned int seconds){
	message_t m;
	m.i1 = seconds;
	return _SYSCALL(SYSCALL_ALARM,&m);
}

pid_t wait(int *wstatus){
	message_t m;
	m.p1 = wstatus;
	return _SYSCALL(SYSCALL_WAIT,&m);
}

pid_t getpid(){
	message_t m;
	if(__pid_cache != 0){
		return __pid_cache;
	}
	__pid_cache = _SYSCALL(SYSCALL_GETPID,&m);
	return __pid_cache;
}

pid_t getppid(){
	message_t m;
	return _SYSCALL(SYSCALL_GETPPID,&m);
}

int kill (pid_t pid, int sig){
	message_t m;
	m.i1 = pid; 
	m.i2 = sig;
	return _SYSCALL(SYSCALL_KILL,&m);
}


