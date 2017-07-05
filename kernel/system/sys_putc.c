#include "../winix.h"

void syscall_putc(proc_t *who, message_t *m){
    kputc(m->i1);
	m->i1 = 0;
	winix_send(who->pid, m);
}