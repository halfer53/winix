#include <lib.h>

/**
 * Exits the current process.
 **/
 int _exit(int status) {
	struct message m;
	m.i1 = status;
	return _syscall(SYSCALL_EXIT,&m);
}

