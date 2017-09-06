#include <lib.h>



unsigned int alarm(unsigned int seconds){
	struct message m;
	m.i1 = seconds;
	return _syscall(SYSCALL_ALARM,&m);
}
