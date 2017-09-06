#include <lib.h>


pid_t wait(int *wstatus){
	struct message m;
	m.p1 = wstatus;
	if(_syscall(SYSCALL_WAIT,&m) < 0)
		return (pid_t)-1;
	if(wstatus != NULL)
		*wstatus = m.i2;
	return (pid_t)m.i1;
}
