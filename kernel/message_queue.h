#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_

#include <sys/ipc.h>

#define MESSAGE_Q_NUM	20

typedef struct mes_q{
	message_t m;
	struct mes_q *next;
}mqueue_t;


// void delayed_send(int who, message_t *message);

void message_queue_main();

void add_message(message_t *m);

#endif

