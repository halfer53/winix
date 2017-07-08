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
	kprintf("Text Segment: 0x%08x - 0x%08x\r\n", &TEXT_BEGIN, &TEXT_END);
	kprintf("Data Segment: 0x%08x - 0x%08x\r\n", &DATA_BEGIN, &DATA_END);
	kprintf("BSS Segment:  0x%08x - 0x%08x\r\n", &BSS_BEGIN, &BSS_END);
	kprintf("Unallocated:  0x%08x - 0x%08x\r\n", FREE_MEM_BEGIN, FREE_MEM_END);
	kprintf("%d kWords Free\r\n", ((unsigned long)(FREE_MEM_END - FREE_MEM_BEGIN)) / 1024);
	//Receive message, do work, repeat.

	getcontext(&recv_ctx);

	while(1) {
		int syscall_ret;
		//get a messa1ge
		winix_receive(&m);
		who_pid = m.src;
		who = get_proc(who_pid);

		//Do the work
		switch(m.type) {
			case SYSCALL_UPTIME:		syscall_ret = do_time(who,&m);		break;
			case SYSCALL_EXIT:			syscall_ret = do_exit(who,&m);		break;
			case SYSCALL_FORK:			syscall_ret = do_fork(who,&m);		break;
			case SYSCALL_EXECVE:		syscall_ret = do_exec(who,&m);		break;
			case SYSCALL_SBRK:			syscall_ret = do_sbrk(who,&m);		break;
			case SYSCALL_BRK:			syscall_ret = do_brk(who,&m);			break;
			case SYSCALL_ALARM:			syscall_ret = do_alarm(who,&m);		break;
			case SYSCALL_SIGNAL:		syscall_ret = do_sigaction(who,&m);	break;
			case SYSCALL_SIGRET:		syscall_ret = do_sigreturn(who,&m);	break;
			case SYSCALL_WAIT:			syscall_ret = do_wait(who,&m);		break;
			case SYSCALL_KILL: 			syscall_ret = do_kill(who,&m);		break;
			case SYSCALL_GETPID:		syscall_ret = do_getpid(who,&m);		break;
			case SYSCALL_GETPPID:		syscall_ret = do_getppid(who,&m);		break;
			case SYSCALL_GETC:			syscall_ret = do_getc(who,&m);		break;
			case SYSCALL_PS:			syscall_ret = do_ps(who,&m);			break;
			case SYSCALL_PUTC:			syscall_ret = do_putc(who,&m);		break;
			case SYSCALL_PRINTF:		syscall_ret = do_printf(who,&m);		break;
			default:
				kprintf("\r\n[SYSTEM] Process \"%s (%d)\" performed unknown system call %d\r\n", who->name, who->pid, m.type);
				exit_proc(who, 1);
				break;
		}

		if(syscall_ret != SUSPEND && syscall_ret != DONOTHING){
			m.i1 = syscall_ret;
			winix_send(who_pid,&m);
		}
	}
}

