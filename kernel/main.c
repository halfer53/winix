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
	// static unsigned long map[32];
	// static unsigned long tmap[32];

	// int index = 0, len = 0;
	// pattern_t *ptn = NULL;
	// int retval = 0;
	//  map[0] = 0xfffff100;
	//  map[1] = 0x00000000;
	// tmap[0] = 0x0000f145;
	// tmap[1] = 0x10000000;
	
	// ptn = extract_pattern(tmap,32,35*1024);
	// index = bitmap_search_pattern(map,32,ptn->pattern,ptn->size);
	// kprintf("pattern %d %x %d\n",index, ptn->pattern,ptn->size);
	// bitmap_set_pattern(map,32,index,ptn->pattern,ptn->size);
	// kprintf("%x %x %x\n",ptn->pattern,map[0],map[1]);
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
	  kblock_overview();
	  kfree(p5);
	  kfree(p6);
	  kfree(p2);
	  kfree(p8);
	  kblock_overview();
	  p0 = krealloc(p0,900);
	  p9 = krealloc(p9,3000);
	  kblock_overview();
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

	//scan memory, initialise FREE_MEM_BEGIN
	Scan_FREE_MEM_BEGIN();

	init_bitmap();
	init_alarm();
	init_mem_table(FREE_MEM_BEGIN);

	//Set up process table
	init_proc();

	//Initialise the system task

	p = new_proc(system_main, SYSTEM_PRIORITY, "SYSTEM");
	assert(p != NULL, "Create sys task");
	p->quantum = 64;

	p = exec_new_proc(init_code,2,0, USER_PRIORITY,"init");
	p->quantum = 1;

	p = exec_new_proc(shell_code,shell_code_length,shell_pc, USER_PRIORITY,"Shell");
	assert(p != NULL, "Create Shell task");
	p->quantum = 2;

	//Initialise exceptions
	init_exceptions();

	//Initialise slab layer
	init_slab(shell_code,shell_code_length);

	//Kick off first task. Note: never returns
	//process_overview();
	sched();
}
