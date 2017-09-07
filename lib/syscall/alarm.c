#include <lib.h>



unsigned int alarm(unsigned int seconds){
    struct message m;
    m.m1_i1 = seconds;
    return _syscall(SYSCALL_ALARM,&m);
}
