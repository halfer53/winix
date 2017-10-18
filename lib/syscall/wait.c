#include <sys/syscall.h>
#include <stddef.h>

pid_t waitpid(pid_t pid, int *wstatus, int options){
    struct message m;
    int ret;
    m.m1_i1 = pid;
    m.m1_i2 = options;
    ret = _syscall(SYSCALL_WAITPID, &m);
    if(ret < 0)
        return (pid_t)-1;
    if(wstatus)
        *wstatus = m.m1_i2;
    return (pid_t)ret;
}

pid_t wait(int *wstatus){
    return waitpid(-1, wstatus, 0);
}
