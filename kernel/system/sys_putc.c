#include "../winix.h"

int do_putc(proc_t *who, message_t *m){
    kputc(m->i1);
    return OK;
}
