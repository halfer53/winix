#include <sys/syscall.h>

int _syscall(int syscall_num, struct message *m){
    m->type = syscall_num;
    return winix_sendrec(SYSTEM_TASK, m);
}


