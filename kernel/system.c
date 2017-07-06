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

void resume_syscall(proc_t *to){
	if(syscall_ctx.who->pid == to->pid && to->flags & RECEIVING && syscall_ctx.interruptted)
		winix_senderr(to->pid);

	syscall_ctx.interruptted = 0;
	// setcontext(&syscall_ctx);
}

void intr_syscall(){
	//if the system is executing a system call, save the system call context
	//and interrupt the System as if it has finished executing the current syscall
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
	int response = 0;

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

		//Do the work
		switch(m.type) {

			case SYSCALL_GETC:		syscall_getc(who,&m);		break;
			case SYSCALL_UPTIME:	syscall_time(who,&m);		break;
			case SYSCALL_EXIT:		syscall_exit(who,&m);		break;
			case SYSCALL_PS:		syscall_ps(who,&m);			break;
			case SYSCALL_FORK:		syscall_fork(who,&m);		break;
			case SYSCALL_EXEC:		syscall_exec(who,&m);		break;
			case SYSCALL_SBRK:		syscall_sbrk(who,&m);		break;
			case SYSCALL_BRK:		syscall_brk(who,&m);		break;
			case SYSCALL_PUTC:		syscall_putc(who,&m);		break;
			case SYSCALL_PRINTF:	syscall_printf(who,&m);		break;
			case SYSCALL_ALARM:		syscall_alarm(who,&m);		break;
			case SYSCALL_SIGNAL:	syscall_sigaction(who,&m);	break;	
			case SYSCALL_SIGRET:	syscall_sigreturn(who,&m);	break;
			case SYSCALL_WAIT:		syscall_wait(who,&m);		break;
			case SYSCALL_KILL: 		syscall_kill(who,&m);		break;
			case SYSCALL_GETPID:	syscall_getpid(who,&m);		break;
			
			
			default:
				kprintf("\r\n[SYSTEM] Process \"%s (%d)\" performed unknown system call %d\r\n", who->name, who->pid, m.type);
				do_exit(who, 1);
				break;
		}	
	}
}

