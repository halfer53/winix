/**
 * 
 * WINIX Inter-process Communication.
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


#include <sys/ipc.h>
#include <stddef.h>

/**
 * Performs a WRAMP system call.
 *
 * Note: implemented in assembly - wramp_syscall.s
 **/
int wramp_syscall(int operation, ...);


/**
 * Receives a message.
 **/
 int winix_receive(struct message *m) {
    //Note: second parameter is currently unused, but is included to simplify kernel code.
    return wramp_syscall(WINIX_RECEIVE, NULL, m);
}
