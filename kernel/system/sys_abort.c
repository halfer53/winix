#include "../winix.h"

int do_abort(proc_t *who, message_t *m){
    cause_sig(who,SIGABRT);
    return DONOTHING;
}
