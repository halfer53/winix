#include "winix.h"

timer_t timer_table[_NTIMERS];
timer_t *pending_alarm[2];
timer_t *free_alarm[2];


void enqueue_alarm(timer_t **q, timer_t *new_alarm){
	if(q[HEAD] == NULL) {
		new_alarm->next = NULL;
		q[HEAD] = q[TAIL] = new_alarm;
	}
	else {
		q[TAIL]->next = new_alarm;
		q[TAIL] = new_alarm;
	}
}

timer_t* dequeue_alarm(timer_t **q){
    timer_t *mq = q[HEAD];

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

void init_alarm(){
    int i;

    free_alarm[HEAD] = free_alarm[TAIL] = NULL;

    for(i =0; i<_NTIMERS; i++){
        timer_table[i].who = NULL;
        timer_table[i].timer = 0;
        enqueue_alarm(free_alarm, &timer_table[i]);
    }
    pending_alarm[HEAD] = pending_alarm[TAIL] = NULL;

}



void deliver_alarm(){
    timer_t *talarm;

    talarm = dequeue_alarm(pending_alarm);
    enqueue_alarm(free_alarm,talarm);
    send_signal(talarm->who,SIGALRM);
}

void clock_handler(){
    system_uptime++;

    if(pending_alarm[HEAD] != NULL 
        && pending_alarm[HEAD]->timer == system_uptime){
        deliver_alarm();
    }
}
