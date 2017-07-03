#include "../winix.h"

void set_signal(proc_t *who, int signum, sighandler_t handler){
    who->sig_table[signum].sa_handler = handler;
}
