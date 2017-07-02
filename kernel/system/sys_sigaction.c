#include "../winix.h"

void set_signal(proc_t *caller, int signum, sighandler_t handler){
    caller->sig_table[signum].sa_handler = handler;
}
