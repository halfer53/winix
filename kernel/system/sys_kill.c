#include "../winix.h"

int sys_kill(proc_t *who, message_t *m){
    do_sigsend(get_proc(m->i1),m->i2);

    return 0;
}