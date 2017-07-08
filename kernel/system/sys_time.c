#include "../winix.h"

int do_time(proc_t *who, message_t *m){
	// winix_send(who->pid, m);
    return system_uptime;
}
