#include "../winix.h"

int do_ps(struct proc *who, struct message *m){
    process_overview();
    // m->i1 = 0;
	// winix_send(who->pid, m);
    return OK;
}

