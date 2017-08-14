/**
 * Main entry point for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include "winix.h"
#include <winix/rex.h>
#include <init_codes.c>
#include <shell_codes.c>

void gpf_handler();
/**
 * Print an error message and lock up the OS... the "Blue Screen of Death"
 *
 * Side Effects:
 *   OS locks up.
 **/
void panic(const char* message) {
	kprintf("\r\nPanic! ");
	kprintf(message);

	while(1) {
		RexParallel->Ctrl = 0;
		RexParallel->LeftSSD = 0x79;  //E
		RexParallel->RightSSD = 0x50; //r
	}
}

/**
 * Asserts that a condition is true.
 * If so, this function has no effect.
 * If not, panic is called with the appropriate message.
 */
void assert(int expression, const char *message) {
	if(!expression) {
		kprintf("\r\nAssertion Failed");
		panic(message);
	}
}

/**
 * Entry point for WINIX.
 **/
void main() {

	struct proc *p = NULL;
	size_t *ptr = NULL;
	void *addr_p = NULL;
	int proc_nr = 0;
	int i=0;

	//Print boot message.
	kprintf("\r\nWINIX v%d.%d\r\n", MAJOR_VERSION, MINOR_VERSION);

	init_bitmap();
	init_mem_table();
	init_proc();

	//Initialise the system task
	p = start_kernel_proc(system_main, SYSTEM_PRIORITY, "SYSTEM");
	assert(p != NULL, "Create sys task");
	p->quantum = 64;

	p = start_user_proc(init_code,2,0, USER_PRIORITY,"init");
	p->quantum = 1;

	p = start_user_proc(shell_code,shell_code_length,shell_pc, USER_PRIORITY,"Shell");
	assert(p != NULL, "Create Shell task");
	p->parent = 1;//hack 
	p->quantum = 2;

	//Initialise exceptions
	init_exceptions();

	//Initialise slab layer
	init_slab(shell_code,shell_code_length);

	//Kick off first task. Note: never returns
	sched();
}
