#include <lib.h>
#include <ucontext.h>


void _ctx_end(ucontext_t *ucp){

	if(ucp->uc_link == NULL)
		exit(EXIT_SUCCESS);
	setcontext(ucp->uc_link);

	//should never get here
	exit(EXIT_FAILURE);
}

void makecontext(ucontext_t *ucp, void (* func)(), int argc, ...){
	int *args = &argc + 1;
	uint32_t **spp;
	uint32_t *sp;
	if(ucp == NULL)
		return;

	ucp->pc = (void (*)())&_ctx_start;
	ucp->regs[11] = (unsigned int)&_ctx_end;

	//allocate stack for the ucp context
	spp = (!ucp->ss_flags) ? &ucp->ss_sp : &ucp->sp;
	*spp -= (argc + 2);
	sp = *spp;

	/**
	 * Arrange the stack as follows:
	 * 	func
	 * 	arg1
	 *  ...
	 *  argn
	 *  ucp
	 *
	 * 	The PC of the ucp will set to _ctx_start (refer to lib/ucontext.s)
	 * 	_ctx_start will pop the top of the stack, which is func. After that, the Stack 
	 * 	will be arranged such that all args are left on the top of the  stack. 
	 * 	Then func() is called.
	 * 	When func() returns, _ctx_start will load value from $12, which is the 
	 *	address of the ctx_end , then call and pass the parameter to _ctx_end
	**/
	*sp++ = (int)func;
	memcpy(sp,args,argc);
	sp += argc;
	*sp = (uint32_t)ucp;

}


