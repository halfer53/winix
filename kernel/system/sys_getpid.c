#include "../winix.h"

pid_t sys_getpid(proc_t *who){
    return who->pid;
}