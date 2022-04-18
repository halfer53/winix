#ifndef _SYS_UCONTEXT_T_
#define _SYS_UCONTEXT_T_ 1

#include <signal.h>

typedef struct stack{
  void  *ss_sp;     /* address of stack */
  int    sstate;  /* Flags */
  size_t ss_size;   /* Number of bytes in stack */
} stack_t;

#define REGS_NR 13

typedef struct mcontext{
    unsigned int regs[REGS_NR];    // values
    unsigned int *sp;
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
