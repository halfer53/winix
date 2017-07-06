#include "../winix.h"

int do_putc(proc_t *who, message_t *m){
    kputc(m->i1);
	// winix_send(who->pid, m);
    m->i1 = 0;
    return OK;
}
