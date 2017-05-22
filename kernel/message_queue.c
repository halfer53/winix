#include "winix.h"

mqueue_t message_table[MESSAGE_Q_NUM];

static mqueue_t *unused_messages[2];

static mqueue_t *queue[2];

void mq_enqueue_head(mqueue_t **q, mqueue_t *mq){
	if(q[HEAD] == NULL) {
		mq->next = NULL;
		q[HEAD] = q[TAIL] = mq;
	}
	else {
		mq->next = q[HEAD];
		q[HEAD] = mq;
	}
}

mqueue_t* mq_dequeue(mqueue_t **q){
	mqueue_t *mq = q[HEAD];

	if(mq == NULL)
		return NULL;

	if(q[HEAD] == q[TAIL]) { //Last item
		q[HEAD] = q[TAIL] = NULL;
	}
	else { //At least one remaining item
		q[HEAD] = mq->next;
	}
	mq->next = NULL;
	return mq;
}

void init_message_queue(){
	int i=0;
	for (i = 0; i < MESSAGE_Q_NUM; ++i)
	{
		mq_enqueue_head(unused_messages,&message_table[i]);
	}
}

void add_message(message_t *m) {
	mqueue_t* mq = mq_dequeue(unused_messages);
	mq->m = *m;
	mq_enqueue_head(queue,mq);
}

void message_queue_main(){
	message_t m;
	init_message_queue();
	while(1){
		if (queue[HEAD] != 0){
			mqueue_t* mq = mq_dequeue(queue);
			m = mq->m;
			winix_send(m.src, &m);
		}
	}
}
