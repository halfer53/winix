#ifndef _UCONTEXT_H_
#define _UCONTEXT_H_ 1

#include <sys/types.h>

#define SS_DISABLE 1
#define SS_ONSTACK 2

#define REGS_NR 13
#define SIGSET_LEN 5

typedef struct _mcontext_t{
	unsigned int regs[REGS_NR];	//Register values
	unsigned int *sp;
	void *ra;
	void (*pc)();
	void *rbase;
	unsigned int *ptable;
	unsigned int cctrl;
	// void *rbase;
	// unsigned int *ptable;
	// unsigned int cctrl;
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
	
	/* Process State */
	unsigned int regs[REGS_NR];	//Register values
	unsigned int *sp;
	void *ra;
	void (*pc)();

	// stack_t uc_stack;
	unsigned int  *ss_sp;     /* address of stack */
	int    ss_flags;  /* Flags */
	size_t ss_size;   /* Number of bytes in stack */

	struct _ucontext_t *uc_link;
}ucontext_t;

void _ctx_start();
int  getcontext(ucontext_t *);
int  setcontext(const ucontext_t *);
void makecontext(ucontext_t *, void (* func)(), int argc, ...);
int  swapcontext(ucontext_t *, const ucontext_t *);

#endif
