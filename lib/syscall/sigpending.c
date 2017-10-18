#include <sys/syscall.h>
#include <signal.h>
#include <stddef.h>
#include <errno.h>

int sigpending(sigset_t *set){
    struct message m;
    int ret;
    ret = _syscall(SYSCALL_SIGPENDING, &m);
    if(set){
        *set = m.m1_i1;
    }else{
        __set_errno(EFAULT);
    }
    return ret;
}

