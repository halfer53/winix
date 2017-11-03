#include <lib.h>
#include <ucontext.h>


void _ctx_end(ucontext_t *ucp){

    if(ucp->uc_link == NULL)
        exit(EXIT_SUCCESS);
    setcontext(ucp->uc_link);

    // should never get here
    exit(EXIT_FAILURE);
}

void makecontext(ucontext_t *ucp, void (*  func)(), int argc, ...){
    int *args = &argc + 1;
    unsigned int *sp;

    if(ucp == NULL || ucp->uc_stack.ss_sp == NULL 
            || ucp->uc_stack.ss_size < MINSIGSTKSZ){
		return;
	}
        

    ucp->uc_mcontext.pc = (void (*)())&_ctx_start;
    ucp->uc_mcontext.regs[7] = (unsigned int)&_ctx_end; // reg 8

    // allocate stack for the ucp context
    sp = (unsigned int*)ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size - 1;
    sp -= argc + 3;
    ucp->uc_mcontext.sp = sp;

    /**
     *  Arrange the stack as follows:
     *  func
     *  argc
     *  arg1
     *  ...
     *  argn
     *  ucp
     *
     *  The PC of the ucp will set to _ctx_start (refer to lib/ucontext.s)
     *  _ctx_start will pop func and argc from the stack, such that 
     *  the stack will be arranged that arg1 to argn are left on the stack.
     *  Then func() is called.
     *  When func() returns, _ctx_start will pop stack by the value of argc,
     *  so that ucp is left on the stack. It then loads value from reg $8, which 
     *  is the address of the _ctx_end, then _ctx_end is called.
     *     
    **/
    *sp++ = (unsigned int)func;
    *sp++ = argc;
    memcpy(sp,args,argc);
    sp += argc;
    *sp = (unsigned int)ucp;
}


