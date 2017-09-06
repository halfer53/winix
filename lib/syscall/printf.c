#include <lib.h>


int printf(const char *format, ...) {
	struct message m;
	m.p1 = (void *)format;
	m.p2 = (void *)((int *)&format+1);
	return _syscall(SYSCALL_PRINTF,&m);
}
