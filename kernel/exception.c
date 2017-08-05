/**
 * Exception-handling routines for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/
#include "winix.h"
#include <winix/rex.h>

//Number of exception sources
#define NUM_HANDLERS 16

static int _irq_count = 0;

//Handler prototypes
PRIVATE void button_handler();
PRIVATE void timer_handler();
PRIVATE void parallel_handler();
PRIVATE void serial1_handler();
PRIVATE void serial2_handler();
PRIVATE void gpf_handler();
PRIVATE void syscall_handler();
PRIVATE void break_handler();
PRIVATE void arith_handler();
PRIVATE void no_handler();

//Table of all handlers.
//Position in the table corresponds to relevant bit of $estat
PRIVATE void (*handlers[])(void) = {
	no_handler, 		//Undefined
	no_handler, 		//Undefined
	no_handler, 		//Undefined
	no_handler, 		//Undefined
	no_handler, 		//IRQ0
	button_handler, 	//IRQ1
	timer_handler, 		//IRQ2
	parallel_handler, 	//IRQ3
	serial1_handler, 	//IRQ4
	serial2_handler, 	//IRQ5
	no_handler, 		//IRQ6
	no_handler, 		//IRQ7
	gpf_handler, 		//GPF
	syscall_handler, 	//SYSCALL
	break_handler, 		//BREAK
	arith_handler 		//ARITH
};

//System uptime, stored as number of timer interrupts since boot
int system_uptime = 0;


int irq_count(){
	return _irq_count;
}
/**
 * User Interrupt Button (IRQ1)
 **/
PRIVATE void button_handler() {
	RexUserInt->Iack = 0;
}

/**
 * Timer (IRQ2)
 *
 * Side Effects:
 *   system_uptime is incremented
 *   scheduler is called (i.e. this handler does not return)
 **/
PRIVATE void timer_handler() {
	RexTimer->Iack = 0;

	//Increment uptime, and check if there is any alarm
	system_uptime++;
	if(next_timeout == system_uptime)
		clock_handler();

	sched();
}

/**
 * Parallel Port (IRQ3)
 **/
PRIVATE void parallel_handler() {
	RexParallel->Iack = 0;
}

/**
 * Serial Port 1 (IRQ4)
 **/
PRIVATE void serial1_handler() {

	RexSp1->Iack = 0;
	// run_message_queue();
}

/**
 * Serial Port 2 (IRQ5)
 **/
PRIVATE void serial2_handler() {
	int stat = RexSp2->Stat;
	if(stat & 1 == 1){
		kprintf("Got %c\n",RexSp2->Rx);
	}else{
		kprintf("Error 0x%08x",RexSp2->Stat);
	}
	RexSp2->Iack = 0;
}

/**
 * General Protection Fault.
 *
 * Side Effects:
 *   Current process is killed.
 *   Scheduler is called (i.e. this handler does not return).
 **/
PRIVATE void gpf_handler() {
	int i=0;
	proc_t *system = get_proc(0);
	if(!isokprocn(current_proc->pid))
		panic("invalid proc");
	//Current process has performed an illegal operation and will be shut down.
	kprintf("\r\n[SYSTEM] Process \"%s (%d)\" Rbase=0x%08x GPF: PC=0x%08x SP=0x%08x.\r\n",
		current_proc->name,
		current_proc->pid,
		current_proc->rbase,
		current_proc->pc,
		current_proc->sp,current_proc);
	kprintf("mIR: 0x%08x\n",*get_physical_addr(current_proc->pc,current_proc));
	kprintf("$1: 0x%08x, $2, 0x%08x, $3, 0x%08x\n",current_proc->regs[0],current_proc->regs[1],current_proc->regs[2]);
	kprintf("$4: 0x%08x, $5, 0x%08x, $6, 0x%08x\n",current_proc->regs[3],current_proc->regs[4],current_proc->regs[5]);
	kprintf("$7: 0x%08x, $8, 0x%08x, $9, 0x%08x\n",current_proc->regs[6],current_proc->regs[7],current_proc->regs[8]);
	kprintf("$10: 0x%08x, $11, 0x%08x, $12, 0x%08x\n",current_proc->regs[9],current_proc->regs[10],current_proc->regs[11]);
	kprintf("$13: 0x%08x, $sp, 0x%08x, $ra, 0x%08x\n",current_proc->regs[12],current_proc->regs[13],current_proc->regs[14]);

	//Kill process and call scheduler.
	KILL_PROC(current_proc,1);
	send_sig(current_proc,SIGSEGV);
	current_proc = NULL;
	sched();
}

/**
 * System Call.
 *
 **/
PRIVATE void syscall_handler() {
	int src, dest, operation;
	message_t *m;
	int *retval;
	proc_t *p;
	ptr_t *sp;
	proc_t *curr = NULL;
	int counter = 0;

	//cast two variables to to size_t to allow addition of two pointer, and then cast back to pointer
	sp = (ptr_t *)((current_proc->sp) + (int)(current_proc->rbase));

	operation = *(sp);				//Operation is the first parameter on the stack
	dest = *(sp+1);				//Destination is second parameter on the stack
	m = (message_t *)(*(sp+ 2) + (int)current_proc->rbase);	//Message pointer is the third parameter on the stack
	m->src = current_proc->pid;			//Don't trust the who to specify their own source process number

	retval = (int*)&current_proc->regs[0];		//Result is returned in register $1
	//Default return value is an error code
	*retval = -1;

	//Decode operation
	switch(operation) {
		case WINIX_SENDREC:
			current_proc->flags |= RECEIVING;
			//fall through to send

		case WINIX_SEND:
			*retval = wini_send(dest, m);
			break;

		case WINIX_RECEIVE:
			*retval = wini_receive(m);
			break;

		case WINIX_NOTIFY:
			*retval = wini_notify(dest,m);
			break;

		default:
			break;
	}

	//A system call could potentially make a high-priority process runnable.
	//Run scheduler.
	sched();
}

/**
 * Breakpoint.
 **/
PRIVATE void break_handler() {
	//TODO: implement handling of breakpoints
}

/**
 * Arithmetic Exception.
 *
 * Side Effects:
 *   Current process is killed, and scheduler is called (i.e. this handler does not return).
 **/
PRIVATE void arith_handler() {
	kprintf("\r\n[SYSTEM] Process \"%s (%d)\" ARITH: PC=0x%08x.\r\n", current_proc->name, current_proc->pid, current_proc->pc);
	send_sig(current_proc,SIGFPE);
	current_proc = NULL;
	sched();
}

/**
 * Generic handler, should never be called.
 *
 * Side Effects:
 *   System Panic! Does not return.
 **/
PRIVATE void no_handler() {
	panic("Unhandled Exception");
}

/**
 * The global exception handler.
 * All relevant exception handlers will be called.
 **/
PRIVATE void exception_handler(int estat) {
	int i;

	//Loop through $estat and call all relevant handlers.
	for(i = NUM_HANDLERS; i; i--) {
		if(estat & (1 << i)) {
			_irq_count++;
			handlers[i]();
		}
	}
	_irq_count = 0;
}

/**
 * Initialises interrupts and exceptions.
 *
 * Side Effects:
 *   System exception handler is initialised.
 *   Timer is configured to generate regular interrupts.
 **/
void init_exceptions() {
	wramp_set_handler(exception_handler);

	RexTimer->Load = 40; //60 Hz
	RexTimer->Ctrl = 3; //Enabled, auto-restart
}
