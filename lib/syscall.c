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
static char* _brk = NULL; //data segment break cache

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
	m.i1 = WINFO_PS;
	return _SYSCALL(SYSCALL_WINFO,&m);
}

int sys_meminfo(){
	struct message m;
	m.i1 = WINFO_MEM;
	return _SYSCALL(SYSCALL_WINFO, &m);
}

pid_t fork(){
	int result;
	struct message m;
	result = _SYSCALL(SYSCALL_FORK,&m);
	if(result == 0)
		_pid = 0; //reset pid cache
	return result;
}

int brk(void *addr){
	struct message m;
	int ret = 0;
	if(addr != _brk){
		m.p1 = addr;
		ret = _SYSCALL(SYSCALL_BRK,&m);
		_brk = m.p1;
		if(m.i1 != 0)
			return -1;
	}
	return ret;
}

void *sbrk(int incr){
	char *newsize, *oldsize;

	if(_brk == NULL){
		brk((void *)-1); //initialise _brk
	}
	oldsize = _brk;
	newsize = _brk + incr;
	if ((incr > 0 && newsize < oldsize) || (incr < 0 && newsize > oldsize))
		return( (void *) -1);
		
	if (brk(newsize) == 0)
		return(oldsize);
	else
		return( (void *) -1);
		
}

int execve(){
	struct message m;
	return _SYSCALL(SYSCALL_EXECVE,&m);
}

int getc(){
	struct message m;
	return _SYSCALL(SYSCALL_GETC,&m);
}

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


