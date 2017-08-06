#ifndef _W_SYSCALL_CTX_H_
#define _W_SYSCALL_CTX_H_ 1

#include <ucontext.h>

typedef struct {
    struct message m;
    struct proc *who;
    ucontext_t ctx;
    int interruptted;
}syscallctx_t;

#endif

