#include <lib.h>

/**
 * Exits the current process.
 **/
 int _exit(int status) {
	struct message m;
	m.m1_i1 = status;
	return _syscall(SYSCALL_EXIT,&m);
}


