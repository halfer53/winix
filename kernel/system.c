/**
 * System task for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include "winix.h"
#include <sys/syscall.h>

/**
 * Entry point for system task.
 **/
void system_main() {
	int counter = 0;
	FREE_MEM_END = 0x1ffff;
	
	//Print Memory Map
	kprintf("Text Segment: %x - %x\r\n", &TEXT_BEGIN, &TEXT_END);
	kprintf("Data Segment: %x - %x\r\n", &DATA_BEGIN, &DATA_END);
	kprintf("BSS Segment:  %x - %x\r\n", &BSS_BEGIN, &BSS_END);
	kprintf("Unallocated:  %x - %x\r\n", FREE_MEM_BEGIN, FREE_MEM_END);
	kprintf("%d kWords Free\r\n", ((unsigned long)(FREE_MEM_END - FREE_MEM_BEGIN)) / 1024);
	//Receive message, do work, repeat.
	while(1) {
		message_t m;
		int who;
		proc_t *caller, *pcurr;
		int response = 0;
		void *ptr = NULL, *ptr2 = NULL;
		ucontext_t context;
		ucontext_t *ucp;

		//Get a message
		winix_receive(&m);
		who = m.src;
		caller = &proc_table[who];
		//kprintf("received from %s, call id %d, operation %d\n",p->name,p->proc_index,m.type );
		//Do the work
		switch(m.type) {

			//Gets the system uptime.
			case SYSCALL_GETC:
				m.i1 = kgetc();
				winix_send(who,&m);
				break;

			case SYSCALL_UPTIME:
				m.i1 = system_uptime;
				winix_send(who, &m);
				break;

			//Exits the current process.
			case SYSCALL_EXIT:
				kprintf("\r\n[SYSTEM] Process \"%s (%d)\" exited with code %d\r\n", caller->name, caller->proc_index, m.i1);
				//TODO: keep process in zombie state until parent calls wait, so the exit value can be retrieved
				end_process(caller);
				break;

			case SYSCALL_PS:
				response = process_overview();
				break;

			case SYSCALL_FORK:
				//fork the calling process
				pcurr = do_fork(caller);
				m.i1 = pcurr->proc_index;
				winix_send(who, &m);

				//send 0 to child
				//NOTE that the forked child should
				//not belong to any scheduling queue, since winix_receive will do that automatically

				m.i1 = 0;
				winix_send(response,&m);

				break;

			case SYSCALL_EXEC:
				response = exec_read_srec(get_proc(who));
				break;

			case SYSCALL_SBRK:
				m.p1 = do_sbrk(caller,m.l1);
				winix_send(who, &m);
				break;

			case SYSCALL_BRK:
				m.i1 = do_brk(caller,m.p1);
				winix_send(who, &m);
				break;

			case SYSCALL_PUTC:
				kputc(m.i1);
				break;

			case SYSCALL_PRINTF:
				//p1: str pointer
				//p2: args pointer
				ptr = ((int *)m.p1) + (int)caller->rbase;
				ptr2 = ((int *)m.p2) + (int)caller->rbase;
				kprintf_vm(&ptr,&ptr2,caller->rbase);
				break;

			case SYSCALL_OPEN:
				
			
			default:
				kprintf("\r\n[SYSTEM] Process \"%s (%d)\" performed unknown system call %d\r\n", caller->name, caller->proc_index, m.type);
				end_process(caller);
				break;
		}
	}
}
