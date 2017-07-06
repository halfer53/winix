#include "../winix.h"

void do_abort(proc_t *who){
    cause_sig(who,SIGABRT);
}

void sys_abort(proc_t *who, message_t *m){
    do_abort(who);
}
