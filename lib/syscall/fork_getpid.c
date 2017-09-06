#include <lib.h>


static pid_t _pid = 0;//pid cache

pid_t fork(){
	int result;
	struct message m;
	result = _syscall(SYSCALL_FORK,&m);
	if(result == 0)
		_pid = 0; //reset pid cache
	return result;
}

pid_t getpid(){
	struct message m;
	if(_pid != 0){
		return _pid;
	}
	_pid = _syscall(SYSCALL_GETPID,&m);
	return _pid;
}

pid_t getppid(){
	struct message m;
	_syscall(SYSCALL_GETPID,&m);
	return m.i2;
}
