#include <lib.h>


/**
 * Get the system uptime.
 **/
 int sys_uptime() {
	struct message m;
	return _SYSCALL(SYSCALL_UPTIME,&m);
}
