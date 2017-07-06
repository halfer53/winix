#include "../winix.h"

void syscall_putc(proc_t *who, message_t *m){
    kputc(m->i1);
	winix_send(who->pid, m);
}
