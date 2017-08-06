#include "../winix.h"

int do_time(struct proc *who, struct message *m){
	// winix_send(who->pid, m);
    return system_uptime;
}
