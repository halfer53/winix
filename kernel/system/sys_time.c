#include "../winix.h"

int do_time(proc_t *who, message_t *m){
    m->i1 = system_uptime;
	// winix_send(who->pid, m);
    return OK;
}
