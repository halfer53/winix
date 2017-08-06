#ifndef _W_MESSAGE_QUEUE_H_
#define _W_MESSAGE_QUEUE_H_ 1

#include <sys/ipc.h>

#define MESSAGE_Q_NUM	20

typedef struct mes_q{
	struct message m;
	struct mes_q *next;
}mqueue_t;


// void delayed_send(int who, struct message *message);

void message_queue_main();

void add_receving_message(struct message *m);
mqueue_t* mq_dequeue(mqueue_t **q);
void mq_enqueue_head(mqueue_t **q, mqueue_t *mq);

extern mqueue_t *receiving_queue[2];
extern mqueue_t *sending_queue[2];


#endif

