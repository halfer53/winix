#include <lib.h>

int _syscall(int syscall_num, struct message *m){
    m->type = syscall_num;
    winix_sendrec(SYSTEM_TASK, m); 
    if(m->m1_i1 < 0){
        __set_errno(-m->m1_i1);
        return -1;
    }
    return m->m1_i1;
}
