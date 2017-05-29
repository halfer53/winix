/**
 * Main entry point for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include "winix.h"
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
	proc_t *system = NULL;
	size_t *ptr = NULL;
	void *addr_p = NULL;
	int pid = 0;
	int i=0;

	//Print boot message.
	kprintf("\r\nWINIX v%d.%d\r\n", MAJOR_VERSION, MINOR_VERSION);

	//scan memory, initialise FREE_MEM_BEGIN
	Scan_FREE_MEM_BEGIN();

	init_holes();
	init_bitmap();
	init_mem_table(FREE_MEM_BEGIN);

	//Set up process table
	init_proc();

	//Initialise the system task
	system = create_system(system_main, SYSTEM_PRIORITY, "SYSTEM");
	assert(system != NULL, "Create sys task");
	system->quantum = 64;

	init = create_init(init_code,2,0);
	init->quantum = 1;

	// p = do_fork(init);
	// p = kexecp(p,message_queue_main, USER_PRIORITY, "MESSAGE");
	// p->quantum = 4;

	//Idle Task
	p = do_fork(init);
	p = kexecp(p,idle_main, IDLE_PRIORITY, "IDLE");
	assert(p != NULL, "Create idle task");
	p->quantum = 1;


	p = do_fork(init);
	p = exec_replace_existing_proc(p,shell_code,shell_code_length,shell_pc, USER_PRIORITY,"Shell");
	assert(p != NULL, "Create Shell task");
	p->quantum = 10;

	
	//Initialise exceptions
	init_exceptions();
	RexSp2->Ctrl = 0x5cd;
	// RexSp1->Ctrl = 0x5cd;
	
	i = bitmap_search(mem_map,MEM_MAP_LEN,1);
	FREE_MEM_BEGIN = i*1024;

	// testkmalloc();

	//Kick off first task. Note: never returns
	// process_overview();
	sched();
}
