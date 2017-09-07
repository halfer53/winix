#include <lib.h>

int _syscall(int syscall_num, struct message *m){
    m->type = syscall_num;
    winix_sendrec(SYSTEM, m);
    if(m->reply_res < 0){
        __set_errno(-m->reply_res);
        return -1;
    }
    return m->reply_res;
}
