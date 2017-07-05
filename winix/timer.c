#include <winix/kernel.h>

timer_t *pending_timers = NULL;

void enqueue_alarm(timer_t *new_alarm){
    timer_t *mq;
    if(new_alarm == NULL)
        return;
    new_alarm->next = pending_timers;
    pending_timers = new_alarm;
}

timer_t* dequeue_alarm(){
    timer_t *mq = pending_timers;

    if(mq)
        pending_timers = mq->next;
    
    mq->next = NULL;
    return mq;
}

