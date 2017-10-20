#include <sys/syscall.h>
#include <signal.h>
#include <stddef.h>
#include <errno.h>

int sigpending(sigset_t *set){
    struct message m;
    int ret;
    ret = _syscall(SIGPENDING, &m);
    if(set)
        *set = m.m1_i1;
    return ret;
}

