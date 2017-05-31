#include <ucontext.h>
#include <stddef.h>
#include <string.h>
#include <type.h>

void _ctx_end(ucontext_t *ucp){

	if(ucp->uc_link == NULL)
		sys_exit(0);
	setcontext(ucp->uc_link);

	//should never get here
	sys_exit(2);
}

void makecontext(ucontext_t *ucp, void (* func)(), int argc, ...){
	int *args = &argc + 1;
	uint32_t **spp;
	uint32_t *sp;
	if(ucp == NULL)
		return;

	ucp->pc = (void (*)())&_ctx_start;

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
	 * 	will be arranged such that all args are left on the stack. Then func() is called.
	 * 	When func() returns, _ctx_start will pop ucp, then call and pass 
	 * 	the parameter to _ctx_end
	**/
	*sp++ = (int)func;
	memcpy(sp,args,argc);
	sp += argc;
	*sp = (uint32_t)ucp;

}


