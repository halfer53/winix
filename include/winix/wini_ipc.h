#ifndef _W_IPC_H_
#define _W_IPC_H_ 1


/**
 * Sends a message
 *
 * Parameters:
 *   m				Pointer to write the message to.
 *
 * Returns:
 *   0 on success
 *   -1 if destination is invalid
 **/
int wini_send(int dest, message_t *m);

/**
 * non-blocking send
 *
 * Parameters:
 *   m				Pointer to write the message to.
 *
 * Returns:
 *   0 on success
 *   -1 if destination is invalid
 **/
int wini_notify(int dest, message_t *m);

/**
 * Receives a message.
 *
 * Parameters:
 *   m				Pointer to write the message to.
 *
 * Returns:			0
 **/
int wini_receive(message_t *m);


#endif
