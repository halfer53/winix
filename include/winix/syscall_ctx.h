#ifndef _W_SYSCALL_CTX_H_
#define _W_SYSCALL_CTX_H_ 1

#include <ucontext.h>

struct syscall_ctx{
    struct message m;
    struct proc *who;
    ucontext_t ctx;
    int interruptted;
};

#endif

