/**
 * 
 * Winix ipcs
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:50
 * 
*/
#ifndef _W_IPC_H_
#define _W_IPC_H_ 1


/**
 * Sends a message
 *
 * Parameters:
 *   m                Pointer to write the message to.
 *
 * Returns:
 *   0 on success
 *   -1 if destination is invalid
 **/
int do_send(int dest, struct message *m);

/**
 * non-blocking send
 *
 * Parameters:
 *   m                Pointer to write the message to.
 *
 * Returns:
 *   0 on success
 *   -1 if destination is invalid
 **/
int do_notify(int src, int dest, struct message *m);

/**
 * Receives a message.
 *
 * Parameters:
 *   m                Pointer to write the message to.
 *
 * Returns:            0
 **/
int do_receive(struct message *m);

/**
 * send used by interrupt handler during exception
 *
 * Parameters:
 *   m                Pointer to write the message to.
 *
 * Returns:
 *   0 on success
 *   -1 if destination is invalid or not in interrupt mode
 **/
int interrupt_send(int dest, struct message* pm);

int syscall_reply(int reply, int dest, struct message* m);
int syscall_reply2(int syscall_num, int reply, int dest, struct message* m);


#endif
