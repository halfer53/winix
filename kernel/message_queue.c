#include "winix.h"

/**

Depreciated, this file is no longer included during compilation


**/
mqueue_t message_table[MESSAGE_Q_NUM];

mqueue_t *unused_messages[2];

mqueue_t *sending_queue[2];
mqueue_t *receiving_queue[2];

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
	receiving_queue[HEAD] = receiving_queue[TAIL] = NULL;
	sending_queue[HEAD] = sending_queue[TAIL] = NULL;
}

void add_receving_message(message_t *m) {
	mqueue_t* mq = mq_dequeue(unused_messages);
	mq->m = *m;
	mq_enqueue_head(receiving_queue,mq);
}

void message_queue_main(){
	message_t m;
	init_message_queue();
	while(1){
		
		if (sending_queue[HEAD] != 0){
			mqueue_t* mq = mq_dequeue(sending_queue);
			m = mq->m;
			mq_enqueue_head(unused_messages,mq);
			winix_send(m.src, &m);
		}
	}
}
