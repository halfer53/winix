#include <lib.h>


/**
 * Get the system uptime.
 **/
 int sys_uptime() {
	struct message m;
	return _syscall(SYSCALL_UPTIME,&m);
}
