/**
 * 
 * Winix system task for processing system calls
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/

#include "winix.h"

PRIVATE struct message m;
PRIVATE int who_proc_nr;
PRIVATE struct proc *who;

PRIVATE ucontext_t recv_ctx;
PRIVATE struct syscall_ctx syscall_ctx;

/**
 * This method resume the system call if it was previously interruppted
 * currently, it just simply return -1 to the user space, and set errno 
 * to EINTR to indicate failure
 * @param to user process to which we send err to
 */
void resume_syscall(struct proc *to){
	if(syscall_ctx.who->proc_nr == to->proc_nr &&
		to->s_flags & RECEIVING && syscall_ctx.interruptted){

		(&syscall_ctx)->m.i1 = EINTR;
		winix_send(to->proc_nr,&syscall_ctx.m);
	}
	syscall_ctx.interruptted = false;
}

/**
 * interrupt the current executing system call
 * it saves the current syscall context into struct syscall_ctx
 * and start receiving syscalls again
 */
void intr_syscall(){
	struct syscall_ctx *ctx = &syscall_ctx;
	if(who_proc_nr){
		ctx->m = m;
		ctx->who = who;
		ctx->interruptted = true;
		setcontext(&recv_ctx);
	}
}

/**
 * public get method for current syscall message
 * @return [description]
 */
struct message *curr_mesg(){
	return &m;
}

/**
 * print sys info of text, data and bss segment size
 */
void print_sysinfo(){
	int free_mem_begin, mem_end;
	free_mem_begin = peek_next_free_page() * PAGE_LEN;
	mem_end = peek_last_free_page() * PAGE_LEN;
	kprintf("\r\nWINIX v%d.%d\r\n", MAJOR_VERSION, MINOR_VERSION);
	kprintf("Text Segment: 0x%08x - 0x%08x\r\n", &TEXT_BEGIN, &TEXT_END);
	kprintf("Data Segment: 0x%08x - 0x%08x\r\n", &DATA_BEGIN, &DATA_END);
	kprintf("BSS Segment:  0x%08x - 0x%08x\r\n", &BSS_BEGIN, &BSS_END);
	kprintf("Unallocated:  0x%08x - 0x%08x\r\n", free_mem_begin, mem_end);
	kprintf("%d kWords Free\r\n", 
	((unsigned int)(mem_end - free_mem_begin + PAGE_LEN)) / PAGE_LEN); //inclusive
}


/**
 * Entry point for system task.
 **/
void system_main() {
	unsigned int null_val = *((unsigned int*)NULL);

	print_sysinfo();
	getcontext(&recv_ctx);

	//Receive message, do work, repeat.
	while(true) {
		
		//get a message
		winix_receive(&m);
		who_proc_nr = m.src;
		who = get_proc(who_proc_nr);

		ASSERT(who != NULL && IS_USER_PROC(who)); 

		//Do the work
		switch(m.type) {
			case SYSCALL_UPTIME:		m.i1 = do_time(who,&m);			break;
			case SYSCALL_EXIT:			m.i1 = do_exit(who,&m);			break;
			case SYSCALL_FORK:			m.i1 = do_fork(who,&m);			break;
			case SYSCALL_EXECVE:		m.i1 = do_exec(who,&m);			break;
			case SYSCALL_BRK:			m.i1 = do_brk(who,&m);			break;
			case SYSCALL_ALARM:			m.i1 = do_alarm(who,&m);		break;
			case SYSCALL_SIGNAL:		m.i1 = do_sigaction(who,&m);	break;
			case SYSCALL_SIGRET:		m.i1 = do_sigreturn(who,&m);	break;
			case SYSCALL_WAIT:			m.i1 = do_wait(who,&m);			break;
			case SYSCALL_KILL: 			m.i1 = do_kill(who,&m);			break;
			case SYSCALL_GETPID:		m.i1 = do_getpid(who,&m);		break;
			case SYSCALL_GETC:			m.i1 = do_getc(who,&m);			break;
			case SYSCALL_WINFO:			m.i1 = do_winfo(who,&m);		break;
			case SYSCALL_PRINTF:		m.i1 = do_printf(who,&m);		break;
			default:
				kinfo("Process \"%s (%d)\" performed unknown system call %d\r\n", 
												who->name, who->proc_nr, m.type);
				m.i1 = ENOSYS;
				break;
		}

		if(m.i1 != SUSPEND && m.i1 != DONOTHING){
			winix_send(who_proc_nr,&m);
		}

		//if this assertion failed, that means null pointer is deferenced
		//during the syscall
		ASSERT(null_val == *(unsigned int*)NULL);

		//reset messages
		memset(&m, 0, sizeof(struct message));
		who_proc_nr = 0;
	}
}

