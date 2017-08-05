#include <kernel/kernel.h>

timer_t *pending_timers = NULL;
clock_t next_timeout;

/*:TODO provide sort of lock mechanism to lock next_timeout before modifying it*/

timer_t* dequeue_alarm(){
    timer_t *mq = pending_timers;

    if(mq){
        pending_timers = mq->next;
        if(pending_timers)
            next_timeout = pending_timers->time_out;
        else
            next_timeout = 0;
    }
    mq->next = NULL;
    return mq;
}

void insert_timer(timer_t *timer){
    timer_t *prev = NULL;
    timer_t *curr = pending_timers;
    clock_t new_timeout = timer->time_out;

    while(curr && curr->time_out < new_timeout){
        curr = curr->next;
        prev = curr;
    }
        
    if(prev){
        prev->next = timer;
        timer->next = curr;        
    }else{
        pending_timers = timer;
    }
    next_timeout = new_timeout;
    timer->flags &= TIMER_INUSE;
}

void remove_timer(timer_t *timer){
    timer_t *prev = NULL;
    timer_t *curr = pending_timers;
    clock_t timeout = timer->time_out;

    while(curr && curr != timer){
        curr = curr->next;
        prev = curr;
    }

    if(prev){
        prev->next = curr->next;
    }else{
        pending_timers = pending_timers->next;
        next_timeout = pending_timers->time_out;
    }
    timer->flags &= ~TIMER_INUSE;
}

