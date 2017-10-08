/**
 * 
 * WINIX Library.
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


#ifndef _WINIX_IPC_H_
#define _WINIX_IPC_H_ 1

#include <signal.h>

typedef struct {int m1i1, m1i2, m1i3; void *m1p1, *m1p2, *m1p3;} mess_1;
typedef struct {long m2l1, m2l2, m2l3, m2l4, m2l5, m2l6;} mess_2;

/**
 * The message structure for IPC
 **/
typedef struct message{
    int src;
    int type;
    union {
        mess_1 m_m1;
        mess_2 m_m2;
    } m_u;
    sighandler_t s1;
} message_t;

/* The following defines provide names for useful members. */
#define m1_i1  m_u.m_m1.m1i1
#define m1_i2  m_u.m_m1.m1i2
#define m1_i3  m_u.m_m1.m1i3
#define m1_p1  m_u.m_m1.m1p1
#define m1_p2  m_u.m_m1.m1p2
#define m1_p3  m_u.m_m1.m1p3

#define m2_l1  m_u.m_m2.m2l1
#define m2_l2  m_u.m_m2.m2l2
#define m2_l3  m_u.m_m2.m2l3
#define m2_l4  m_u.m_m2.m2l4
#define m2_l5  m_u.m_m2.m2l5
#define m2_l6  m_u.m_m2.m2l6

#define reply_res	type

/**
 * Magic Numbers for send/receive
 **/
#define WINIX_SEND        	0x13370001
#define WINIX_RECEIVE    	0x13370002
#define WINIX_SENDREC    	0x13370003
#define WINIX_NOTIFY    	0x13370004

/**
 * Boot Image Task Numbers
 **/
#define SYSTEM_TASK         0


/**
 * Sends a message to the destination process
 **/
int winix_send(int dest, struct message *m);

/**
 * Receives a message.
 **/
int winix_receive(struct message *m);

/**
 * Sends and receives a message to/from the destination process.
 *
 * Note: overwrites m with the reply message.
 **/
int winix_sendrec(int dest, struct message *m);

#endif
