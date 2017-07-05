#ifndef _W_SYSCALL_CTX_H_
#define _W_SYSCALL_CTX_H_ 1

#include <ucontext.h>

typedef struct {
    message_t m;
    proc_t *who;
    ucontext_t ctx;
    int interruptted;
}syscallctx_t;

#endif

