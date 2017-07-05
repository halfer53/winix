#include "../winix.h"

void syscall_time(proc_t *who, message_t *m){
    m->i1 = system_uptime;
	winix_send(who->pid, m);
}
