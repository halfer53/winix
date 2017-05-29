#ifndef _UCONTEXT_H_
#define _UCONTEXT_H_

#include <type.h>

#define SS_DISABLE 1
#define SS_ONSTACK 2


#define REGS_NR 13
#define SIGSET_LEN 5

typedef struct _mcontext_t{
	unsigned long regs[REGS_NR];	//Register values
	void *ra;
	void (*pc)();
	unsigned long *sp;
	// void *rbase;
	// unsigned long *ptable;
	// unsigned long cctrl;
}mcontext_t;


// typedef struct _sigset_t{
// 	int signals[SIGSET_LEN];
// }

typedef struct {
   void  *ss_sp;     /* address of stack */
   int    ss_flags;  /* Flags */
   size_t ss_size;   /* Number of bytes in stack */
} stack_t;

typedef struct _ucontext_t{
	mcontext_t uc_context;
	stack_t uc_stack;
	struct _ucontext_t *next;
}ucontext_t;

int  getcontext(ucontext_t *);
int  setcontext(const ucontext_t *);
void makecontext(ucontext_t *, (void *)(), int, ...);
int  swapcontext(ucontext_t *, const ucontext_t *);

#endif