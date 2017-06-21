/**
 * Exception-handling routines for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/
#include "winix.h"

//Number of exception sources
#define NUM_HANDLERS 16

//Handler prototypes
static void button_handler();
static void timer_handler();
static void parallel_handler();
static void serial1_handler();
static void serial2_handler();
static void gpf_handler();
static void syscall_handler();
static void break_handler();
static void arith_handler();
static void no_handler();

//Table of all handlers.
//Position in the table corresponds to relevant bit of $estat
static void (*handlers[])(void) = {
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


/**
 * User Interrupt Button (IRQ1)
 **/
static void button_handler() {
	RexUserInt->Iack = 0;
}

/**
 * Timer (IRQ2)
 *
 * Side Effects:
 *   system_uptime is incremented
 *   scheduler is called (i.e. this handler does not return)
 **/
static void timer_handler() {
	RexTimer->Iack = 0;

	//Increment uptime, and check if there is any alarm
	clock_handler();
	// system_uptime++;

	sched();
}

/**
 * Parallel Port (IRQ3)
 **/
static void parallel_handler() {
	RexParallel->Iack = 0;
}

/**
 * Serial Port 1 (IRQ4)
 **/
static void serial1_handler() {

	// int stat = RexSp1->Stat;
	// mqueue_t *mq;
	// if(stat & 1 == 1){
	// 	if (receiving_queue[HEAD] != NULL){
	// 		mq = mq_dequeue(receiving_queue);
	// 		(mq->m).i1 = RexSp1->Rx;
	// 		// kprintf("E Get %c",m.i1);
	// 		mq_enqueue_head(sending_queue,mq);
	// 		RexSp1->Iack = 0;
	// 	}else{
	// 		kprintf("Err waiting \n");
	// 	}
	// }
	RexSp1->Iack = 0;
	// run_message_queue();
}

/**
 * Serial Port 2 (IRQ5)
 **/
static void serial2_handler() {
	int stat = RexSp2->Stat;
	if(stat & 1 == 1){
		kprintf("Got %c\n",RexSp2->Rx);
	}else{
		kprintf("Error %x",RexSp2->Stat);
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
static void gpf_handler() {
	int i=0;
	proc_t *system = get_proc(0);
	//Current process has performed an illegal operation and will be shut down.
	kprintf("\r\n[SYSTEM] Process \"%s (%d)\" Rbase=%x GPF: PC=%x SP=%x.\r\n",
		current_proc->name,
		current_proc->proc_index,
		current_proc->rbase,
		current_proc->pc,
		current_proc->sp,current_proc);
	kprintf("ptable: %x %x %x\n",current_proc->ptable[0],current_proc->ptable[1],current_proc->ptable[2]);
	kprintf("$1: %x, $2, %x, $3, %x\n",current_proc->regs[0],current_proc->regs[1],current_proc->regs[2]);
	kprintf("$4: %x, $5, %x, $6, %x\n",current_proc->regs[3],current_proc->regs[4],current_proc->regs[5]);
	kprintf("$7: %x, $8, %x, $9, %x\n",current_proc->regs[6],current_proc->regs[7],current_proc->regs[8]);
	kprintf("$10: %x, $11, %x, $12, %x\n",current_proc->regs[9],current_proc->regs[10],current_proc->regs[11]);
	kprintf("$13: %x, $sp, %x, $ra, %x\n",current_proc->regs[12],current_proc->regs[13],current_proc->regs[14]);

	//Kill process and call scheduler.
	end_process(current_proc);
	current_proc = NULL;
	sched();
}

/**
 * System Call.
 *
 **/
static void syscall_handler() {
	int src, dest, operation;
	message_t *m;
	int *retval;
	proc_t *p;
	size_t *sp;
	proc_t *curr = NULL;
	char *op_name;
	int counter = 0;

	//cast two variables to to size_t to allow addition of two pointer, and then cast back to pointer
	sp = (size_t *)((size_t)(current_proc->sp) + (size_t)(current_proc->rbase));

	//printProceInfo(current_proc);
	if (((size_t)current_proc->rbase) != 0) {
		*(sp+2) += (size_t)current_proc->rbase;
	}

	operation = *(sp);				//Operation is the first parameter on the stack
	dest = *(sp+1);				//Destination is second parameter on the stack
	m = *(message_t **)(sp+ 2);	//Message pointer is the third parameter on the stack
	m->src = current_proc->proc_index;			//Don't trust the caller to specify their own source process number

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
static void break_handler() {
	//TODO: implement handling of breakpoints
}

/**
 * Arithmetic Exception.
 *
 * Side Effects:
 *   Current process is killed, and scheduler is called (i.e. this handler does not return).
 **/
static void arith_handler() {
	kprintf("\r\n[SYSTEM] Process \"%s (%d)\" ARITH: PC=%x.\r\n", current_proc->name, current_proc->proc_index, current_proc->pc);
	end_process(current_proc);
	current_proc = NULL;
	sched();
}

/**
 * Generic handler, should never be called.
 *
 * Side Effects:
 *   System Panic! Does not return.
 **/
static void no_handler() {
	panic("Unhandled Exception");
}

/**
 * The global exception handler.
 * All relevant exception handlers will be called.
 **/
static void exception_handler(int estat) {
	int i;

	//Loop through $estat and call all relevant handlers.
	for(i = NUM_HANDLERS; i; i--) {
		if(estat & (1 << i)) {
			handlers[i]();
		}
	}
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
