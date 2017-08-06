#include "../winix.h"

int do_time(struct proc *who, struct message *m){
	// winix_send(who->proc_nr, m);
    return system_uptime;
}
