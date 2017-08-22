#ifndef _UCONTEXT_H_
#define _UCONTEXT_H_ 1

#include <sys/types.h>
#include <signal.h>
#include <sys/ucontext.h>

void _ctx_start();
int  getcontext(ucontext_t *);
int  setcontext(const ucontext_t *);
void makecontext(ucontext_t *, void (* func)(), int argc, ...);
int  swapcontext(ucontext_t *, const ucontext_t *);

#endif
