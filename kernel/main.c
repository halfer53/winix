/**
 * Main entry point for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include "winix.h"
#include <exec_codes.c>

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

	proc_t *p = NULL;
	size_t *ptr = NULL;
	void *addr_p = NULL;
	int pid = 0;
	int i=0;

	//Print boot message.
	kprintf("\r\nWINIX v%d.%d\r\n", MAJOR_VERSION, MINOR_VERSION);

	//scan memory, initialise FREE_MEM_BEGIN and FREE_MEM_END
	Scan_FREE_MEM_BEGIN();

	init_memory();

	//Set up process table

	init_proc();

	//Initialise the system task

	p = new_proc(system_main, SYSTEM_PRIORITY, "SYSTEM");
	assert(p != NULL, "Create sys task");
	p->quantum = 64;

	//Idle Task

	p = new_proc(idle_main, IDLE_PRIORITY, "IDLE");
	assert(p != NULL, "Create idle task");


	p = exec_new_proc(init_code,2,0, USER_PRIORITY,"init");
	p->quantum = 1;

	pid = fork_proc(p);

	// pid = fork_proc(p);
	p = get_proc(pid);

	// p = new_proc(shell_main, USER_PRIORITY, "Shell");
	// assert(p != NULL, "Create Shell task");
	// p->quantum = 4;

	p = exec_replace_existing_proc(p,shell_code,shell_code_length,shell_pc, USER_PRIORITY,"Shell");
	assert(p != NULL, "Create Shell task");
	p->quantum = 10;

	//Rocks game
	/*p = new_proc(rocks_main, USER_PRIORITY, "Rocks");
	assert(p != NULL, "Create rocks task");
	p->quantum = 4;*/

	//Parallel test program
	// p = new_proc(parallel_main, USER_PRIORITY, "Parallel");
	// assert(p != NULL, "Create parallel task");
	// p->quantum = 1;
	//###########################################

	//Initialise exceptions
	init_exceptions();
	
	init_bitmap();
	init_mem_table();

	//Kick off first task. Note: never returns

	//process_overview();
	sched();
}
