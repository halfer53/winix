#include "../winix.h"

int do_ps(proc_t *who, message_t *m){
    process_overview();
    m->i1 = 0;
	// winix_send(who->pid, m);
    return OK;
}

