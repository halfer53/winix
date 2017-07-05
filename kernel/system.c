/**
 * System task for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include "winix.h"

message_t m;
int who_pid;
proc_t *who;

ucontext_t recv_ctx;
syscallctx_t syscall_ctx;

void send_err(proc_t *to){
	message_t tmesg;
	memset(&tmesg,-1,MESSAGE_LEN);
	winix_send(to->pid,&tmesg);
}

void resume_syscall(proc_t *to){
	if(syscall_ctx.who->pid == to->pid && to->flags & RECEIVING && syscall_ctx.interruptted)
		send_err(to);

	syscall_ctx.interruptted = 0;
	// setcontext(&syscall_ctx);
}

void intr_syscall(){
	//if the system is executing a system call, save the system call context
	//and restart the System as if it has finished executing the current syscall
	if(!get_proc(SYSTEM_TASK)->flags){
		syscall_ctx.m = m;
		syscall_ctx.who = who;
		syscall_ctx.interruptted = 1;
	}
	setcontext(&recv_ctx);
}

message_t *curr_mesg(){
	return &m;
}
/**
 * Entry point for system task.
 **/
void system_main() {
	int counter = 0;
	proc_t *pcurr;
	int response = 0;
	void *ptr = NULL, *ptr2 = NULL;

	FREE_MEM_END = 0x1ffff;
	
	//Print Memory Map
	kprintf("Text Segment: %x - %x\r\n", &TEXT_BEGIN, &TEXT_END);
	kprintf("Data Segment: %x - %x\r\n", &DATA_BEGIN, &DATA_END);
	kprintf("BSS Segment:  %x - %x\r\n", &BSS_BEGIN, &BSS_END);
	kprintf("Unallocated:  %x - %x\r\n", FREE_MEM_BEGIN, FREE_MEM_END);
	kprintf("%d kWords Free\r\n", ((unsigned long)(FREE_MEM_END - FREE_MEM_BEGIN)) / 1024);
	//Receive message, do work, repeat.

	getcontext(&recv_ctx);
	while(1) {
		//get a messa1ge
		winix_receive(&m);
		who_pid = m.src;
		who = get_proc(who_pid);

		// kprintf("from %d op %d",who->pid,m.type );
		// if(get_proc(0)->sender_q->pid < NUM_PROCS)
		// 	kprintf(" next %d op %d ",get_proc(0)->sender_q->pid,get_proc(0)->sender_q->message);
		// kputc('\n');

		//Do the work
		switch(m.type) {

			//Gets the system uptime.
			case SYSCALL_GETC:
				m.i1 = kgetc();
				winix_send(who_pid,&m);
				break;

			case SYSCALL_UPTIME:
				m.i1 = system_uptime;
				winix_send(who_pid, &m);
				break;

			//Exits the current process.
			case SYSCALL_EXIT:
				do_exit(who,&m);
				break;

			case SYSCALL_PS:
				process_overview();
				break;

			case SYSCALL_FORK:
				pcurr = do_fork(who);
				m.i1 = pcurr->pid;
				winix_send(who_pid, &m);
				
				//send 0 to child
				m.i1 = 0;
				winix_send(pcurr->pid,&m);
				break;

			case SYSCALL_EXEC:
				response = exec_read_srec(get_proc(who_pid));
				break;

			case SYSCALL_SBRK:
				m.p1 = do_sbrk(who,m.i1);
				winix_send(who_pid, &m);
				break;

			case SYSCALL_BRK:
				m.i1 = do_brk(who,m.p1);
				winix_send(who_pid, &m);
				break;

			case SYSCALL_PUTC:
				kputc(m.i1);
				break;

			case SYSCALL_PRINTF:
				//p1: str pointer
				//p2: args pointer
				ptr = get_physical_addr(m.p1,who);
				ptr2 = get_physical_addr(m.p2,who);
				kprintf_vm(ptr,ptr2,who->rbase);
				break;

			case SYSCALL_ALARM:
				// DEBUG_IPC = 10;
				// DEBUG_SCHED = 20;
				sys_alarm(who,m.i1);
				break;

			case SYSCALL_SIGNAL:
				set_signal(who,m.i1,m.s1);
				break;
				
			case SYSCALL_SIGRET:
				do_sigreturn(who,m.i1);
				break;
			
			case SYSCALL_WAIT:
				do_wait(who,&m);
				break;

			case SYSCALL_GETPID:
				m.i1 = who_pid;
				winix_send(who_pid,&m);
				break;
			
			default:
				kprintf("\r\n[SYSTEM] Process \"%s (%d)\" performed unknown system call %d\r\n", who->name, who->pid, m.type);
				end_process(who);
				break;
		}
	}
}

