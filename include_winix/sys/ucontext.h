#ifndef _SYS_UCONTEXT_T_
#define _SYS_UCONTEXT_T_ 1

#include <winix/type.h>

#ifdef __wramp__
#include <signal.h>
#else
#include <bits/types/stack_t.h>
#endif

#define REGS_NR 13

typedef struct mcontext{
    reg_t regs[REGS_NR];    // values
    reg_t *sp;
    void *ra;
    void (*pc)();
}mcontext_t;

typedef struct ucontext{
    mcontext_t uc_mcontext;
    stack_t uc_stack;
    sigset_t uc_sigmask;
    struct ucontext *uc_link;
}ucontext_t;


#endif
