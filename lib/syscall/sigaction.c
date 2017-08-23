#include <lib.h>



sighandler_t signal(int signum, sighandler_t handler){
	struct message m;
	m.i1 = signum;
	m.s1 = handler;
	_SYSCALL(SYSCALL_SIGNAL,&m);
	return m.s1;
}
