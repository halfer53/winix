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

static pid_t _pid = 0;//pid cache

int _SYSCALL(int syscall_num, struct message *m){
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
	struct message m;
	return _SYSCALL(SYSCALL_UPTIME,&m);
}

/**
 * Exits the current process.
 **/
int _exit(int status) {
	struct message m;
	m.i1 = status;
	return _SYSCALL(SYSCALL_EXIT,&m);
}

int sys_ps(){
	struct message m;
	return _SYSCALL(SYSCALL_PS,&m);
}

pid_t fork(){
	int result;
	struct message m;
	result = _SYSCALL(SYSCALL_FORK,&m);
	if(result == 0)
		_pid = 0; //reset pid cache
	return result;
}

void *sbrk(size_t size){
	struct message m;
	m.i1 = size;
	_SYSCALL(SYSCALL_SBRK,&m);
	return m.p1;
}

int brk(void *addr){
	struct message m;
	m.p1 = addr;
	return _SYSCALL(SYSCALL_BRK,&m);
}

int exec(){
	struct message m;
	return _SYSCALL(SYSCALL_EXECVE,&m);
}


int getc(){
	struct message m;
	return _SYSCALL(SYSCALL_GETC,&m);
}

// int putc(int i){
// 	struct message m;
// 	m.i1 = i;
// 	return _SYSCALL(SYSCALL_PUTC,&m);
// }

int printf(const char *format, ...) {
	struct message m;
	m.p1 = (void *)format;
	m.p2 = (void *)((int *)&format+1);
	return _SYSCALL(SYSCALL_PRINTF,&m);
}


sighandler_t signal(int signum, sighandler_t handler){
	struct message m;
	m.i1 = signum;
	m.s1 = handler;
	_SYSCALL(SYSCALL_SIGNAL,&m);
	return m.s1;
}

unsigned int alarm(unsigned int seconds){
	struct message m;
	m.i1 = seconds;
	return _SYSCALL(SYSCALL_ALARM,&m);
}

pid_t wait(int *wstatus){
	struct message m;
	m.p1 = wstatus;
	return _SYSCALL(SYSCALL_WAIT,&m);
}

pid_t getpid(){
	struct message m;
	if(_pid != 0){
		return _pid;
	}
	_pid = _SYSCALL(SYSCALL_GETPID,&m);
	return _pid;
}

pid_t getppid(){
	struct message m;
	return _SYSCALL(SYSCALL_GETPPID,&m);
}

int kill (pid_t pid, int sig){
	struct message m;
	m.i1 = pid; 
	m.i2 = sig;
	return _SYSCALL(SYSCALL_KILL,&m);
}


