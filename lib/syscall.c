/**
 * WINIX System Calls.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#include <sys/ipc.h>
#include <sys/syscall.h>

/**
 * Get the system uptime.
 **/
int sys_uptime() {
	int response = 0;
	message_t m;

	m.type = SYSCALL_UPTIME;
	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
	return m.i1;
}

/**
 * Exits the current process.
 **/
int sys_exit(int status) {
	int response = 0;
	message_t m;

	m.type = SYSCALL_EXIT;
	m.i1 = status;
	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
	return m.i1;
}

int sys_process_overview(){
	int response = 0;
	message_t m;

	m.type = SYSCALL_PROCESS_OVERVIEW;
	response = winix_send(SYSTEM_TASK, &m); //TODO: error checking
	return 0;
}

int fork(){
	int response = 0;
	message_t m;

	m.type = SYSCALL_FORK;
	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
	response = m.i1;
	return m.i1;
}

int fork_pid(int proc_index){
	int response = 0;
	message_t m;

	m.type = SYSCALL_FORK_PID;
	m.i1 = proc_index;
	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking

	return m.i1;
}

int exec(){
	int response = 0;
	message_t m;

	m.type = SYSCALL_EXEC;
	response = winix_send(SYSTEM_TASK, &m); //TODO: error checking
	return 0;
}

void *malloc(unsigned long size){
	int response = 0;
	message_t m;

	m.type = SYSCALL_MALLOC;
	m.s1 = size;
	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
	return m.p1;
}

void free(void *ptr){
	int response = 0;
	message_t m;

	m.type = SYSCALL_FREE;
	m.p1 = ptr;
	response = winix_send(SYSTEM_TASK, &m); //TODO: error checking
}

void holes_overview(){
	int response = 0;
	message_t m;

	m.type = SYSCALL_HOLE_OVERVIEW;
	response = winix_send(SYSTEM_TASK, &m); //TODO: error checking
}

int getc(){
	int response = 0;
	message_t m;

	m.type = SYSCALL_GETC;
	//printf("get c %d\n",m.type );
	response = winix_sendrec(SYSTEM_TASK, &m); //TODO: error checking
	return m.i1;
}

void putc(int i){
	int response = 0;
	message_t m;

	m.type = SYSCALL_PUTC;
	//printf("putc syscall id %d, val %c\n",m.type,(char)i );
	m.i1 = i;
	response = winix_send(SYSTEM_TASK, &m); //TODO: error checking
	return;
}
