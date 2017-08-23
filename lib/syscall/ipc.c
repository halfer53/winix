/**
 * 
 * WINIX Inter-process Communication.
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * @create date 2016-09-19
 * @modify date 2017-08-23 06:03:34
*/


#include <sys/ipc.h>
#include <stddef.h>

/**
 * Performs a WRAMP system call.
 *
 * Note: implemented in assembly - wramp_syscall.s
 **/
int wramp_syscall(int operation, ...);

/**
 * Sends a message to the destination process
 **/
int winix_send(int dest, struct message *m) {
	return wramp_syscall(WINIX_SEND, dest, m);
}

/**
 * Receives a message.
 **/
int winix_receive(struct message *m) {
	//Note: second parameter is currently unused, but is included to simplify kernel code.
	return wramp_syscall(WINIX_RECEIVE, NULL, m);
}

/**
 * Sends and receives a message to/from the destination process.
 *
 * Note: overwrites m with the reply message.
 **/
int winix_sendrec(int dest, struct message *m) {
	return wramp_syscall(WINIX_SENDREC, dest, m);
}

/**
 * Non-blocking send
 *
 **/
int winix_notify(int dest, struct message *m) {
	return wramp_syscall(WINIX_NOTIFY, dest, m);
}
