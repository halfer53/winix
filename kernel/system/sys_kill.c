#include "../winix.h"

int syscall_kill(proc_t *who, message_t *m){
    do_sigsend(get_runproc(m->i1),m->i2);

    return 0;
}