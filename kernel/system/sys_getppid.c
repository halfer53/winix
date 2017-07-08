#include "../winix.h"

int do_getppid(proc_t *who, message_t *m){
    // m->i1 = get_proc(who->parent)->pid;
    return get_proc(who->parent)->pid;
}
