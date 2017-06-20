/**
 * WINIX Library.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#ifndef _WINIX_IPC_H_
#define _WINIX_IPC_H_

#include <signal.h>

typedef struct {int m1i1, m1i2, m1i3; void *m1p1, *m1p2, *m1p3;} mess_1;
typedef struct {int m2i1, m2i2, m2i3; long m2l1;unsigned long m2ul1; void *m2p1; short m2s1;} mess_2;
/**
 * The message structure for IPC
 **/
typedef struct {
	int src;
	int type;
	int i1, i2, i3;
	void *p1, *p2, *p3;
	sighandler_t s1;
} message_t;

#define MESSAGE_LEN	9


/**
 * Magic Numbers for send/receive
 **/
#define WINIX_SEND		0x13370001
#define WINIX_RECEIVE	0x13370002
#define WINIX_SENDREC	0x13370003
#define WINIX_NOTIFY	0x13370004

/**
 * Boot Image Task Numbers
 **/
#define SYSTEM_TASK 0


/**
 * Sends a message to the destination process
 **/
int winix_send(int dest, message_t *m);
int winix_sendonce(int dest, message_t *m);

/**
 * Receives a message.
 **/
int winix_receive(message_t *m);

/**
 * Sends and receives a message to/from the destination process.
 *
 * Note: overwrites m with the reply message.
 **/
int winix_sendrec(int dest, message_t *m);

#endif
