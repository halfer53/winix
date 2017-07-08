#include "../winix.h"

int do_getpid(proc_t *who, message_t *m){
    return who->pid;
}
