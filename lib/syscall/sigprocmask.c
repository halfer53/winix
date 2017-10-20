#include <sys/syscall.h>
#include <signal.h>
#include <stddef.h>
#include <errno.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset){
    struct message m;
    int ret;
    if(set == NULL){
        __set_errno(EFAULT);
        return -1;
    }
    m.m1_i1 = how;
    m.m1_i2 = *set;
    ret = _syscall(SIGPROCMASK, &m);
    if(oldset)
        *oldset = m.m1_i1;
    else
        __set_errno(EFAULT);
    return ret;
}

