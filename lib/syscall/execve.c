#include <lib.h>


int execve(){
	struct message m;
	return _SYSCALL(SYSCALL_EXECVE,&m);
}
