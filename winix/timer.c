/**
 * 
 * Winix timer module
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:13:01
 * @modify date 2017-08-23 06:13:01
*/
#include <kernel/kernel.h>

//a linked list of pending timers to be alarmed
PRIVATE struct timer *pending_timers = NULL;
clock_t next_timeout = 0;

/*:TODO provide sort of lock mechanism to lock next_timeout before modifying it*/

/**
 * get the most recent alarm in the linkedlist
 * @return 
 */
struct timer* dequeue_alarm(){
    struct timer *mq = pending_timers;

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

/**
 * insert a new timer into the system, 
 * insertion sort is used to sort the timers
 * @param timer 
 */
void insert_timer(struct timer *timer){
    struct timer *prev = NULL;
    struct timer *curr = pending_timers;
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

/**
 * remove timer from the pending_timers
 * @param timer 
 */
void remove_timer(struct timer *timer){
    struct timer *prev = NULL;
    struct timer *curr = pending_timers;
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

