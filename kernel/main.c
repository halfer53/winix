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

void testbitmap(){
	unsigned long pattern = 0xa47b4000;
	int retval = 0;
	mem_map[1] = 0xf84b0000;
	retval = bitmap_search_pattern(mem_map,MEM_MAP_LEN,pattern,20);
	kprintf("Got %d\n",retval);
}

void testkmalloc(){
	void *p0 = kmalloc(512);
	  void *p1 = kmalloc(512);
	  void *p2 = kmalloc(1024);
	  void *p3 = kmalloc(512);
	  void *p4 = kmalloc(1024);
	  void *p5 = kmalloc(2048);
	  void *p6 = kmalloc(512);
	  void *p7 = kmalloc(1024);
	  void *p8 = kmalloc(512);
	  void *p9 = kmalloc(1024);
	  block_overview();
	  kfree(p5);
	  kfree(p6);
	  kfree(p2);
	  kfree(p8);
	  block_overview();
	  p0 = krealloc(p0,900);
	  p9 = krealloc(p9,3000);
	  block_overview();
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
	proc_t *init = NULL;
	size_t *ptr = NULL;
	void *addr_p = NULL;
	int pid = 0;
	int i=0;

	//Print boot message.
	kprintf("\r\nWINIX v%d.%d\r\n", MAJOR_VERSION, MINOR_VERSION);

	//scan memory, initialise FREE_MEM_BEGIN
	Scan_FREE_MEM_BEGIN();

	init_memory();
	init_bitmap();

	//Set up process table
	init_proc();

	//Initialise the system task
	p = create_system(system_main, SYSTEM_PRIORITY, "SYSTEM");
	assert(p != NULL, "Create sys task");
	p->quantum = 64;

	init = create_init(init_code,2,0);
	init->quantum = 1;

	p = _fork(init);
	p = kexecp(p,message_queue_main, USER_PRIORITY, "MESSAGE");
	assert(p != NULL, "Create Message Queue");
	p->quantum = 4;
	
	//Idle Task
	p = _fork(init);
	p = kexecp(p,idle_main, IDLE_PRIORITY, "IDLE");
	assert(p != NULL, "Create idle task");

	p = _fork(init);
	p = exec_replace_existing_proc(p,shell_code,shell_code_length,shell_pc, USER_PRIORITY,"Shell");
	assert(p != NULL, "Create Shell task");
	p->quantum = 10;

	//Initialise exceptions
	init_exceptions();
	RexSp2->Ctrl = 0x5cd;
	RexSp1->Ctrl = 0x5cd;
	init_mem_table(FREE_MEM_BEGIN);

	// testkmalloc();
	testbitmap();
	//Kick off first task. Note: never returns
	sched();
}
