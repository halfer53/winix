#include <sys/syscall.h>
#include <stddef.h>

pid_t wait(int *wstatus){
    struct message m;
    int ret;
    m.m1_p1 = wstatus;
    if((ret = _syscall(SYSCALL_WAIT,&m)) < 0)
        return (pid_t)-1;
    if(wstatus != NULL)
        *wstatus = m.m1_i2;
    return (pid_t)ret;
}
