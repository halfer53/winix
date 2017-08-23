#include <lib.h>

/**
 * Exits the current process.
 **/
 int _exit(int status) {
	struct message m;
	m.i1 = status;
	return _SYSCALL(SYSCALL_EXIT,&m);
}

