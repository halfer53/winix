#include <lib.h>

int sigpending(sigset_t *set){
    struct message m;
    m.m1_p1 = set;
    return _syscall(SYSCALL_SIGPENDING, &m);
}

