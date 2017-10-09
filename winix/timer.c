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
 * 
*/
#include <kernel/kernel.h>
#include <kernel/clock.h>
#include <kernel/exception.h>


//a linked list of pending timers to be alarmed
PRIVATE struct timer *pending_timers = NULL;

void kprint_timers(){
    struct timer *mq = pending_timers;
    
    while(mq != NULL)
    {
        kinfo("timer timeout %d from %d\n",mq->time_out, mq->proc_nr);
        mq = mq->next;
    }
    kinfo("next timeout %d\n",next_timeout);
}

int new_timer(struct timer* curr, clock_t timeout, timerhandler_t watchdog){

    if(timeout == 0)
        return ERR;

    if( !(curr->flags & TIMER_INUSE) ){
        curr->time_out = get_uptime() + timeout;
        curr->proc_nr = SYSTEM;
        curr->handler = watchdog;
        insert_timer(curr);
        return OK;
    }
    
    // PANIC("No timer left");
    return ERR;
}


/**
 * get the most recent alarm in the linkedlist
 * @return 
 */
struct timer* dequeue_alarm(){
    struct timer* mq;
    mq = pending_timers;
    if(mq){
        pending_timers = mq->next;
        if(pending_timers)
            next_timeout = pending_timers->time_out;
        else
            next_timeout = 0;
    }else{
        PANIC("No alarm in the queue");
    }
    mq->flags &= ~TIMER_INUSE;
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

    disable_interrupt();
    while(curr && curr->time_out < new_timeout){
        prev = curr;
        curr = curr->next;
    }

    if(prev){
        prev->next = timer;
        timer->next = curr;
    }else{
        timer->next = pending_timers;
        pending_timers = timer;
        next_timeout = pending_timers->time_out;
    }
    timer->flags &= TIMER_INUSE;

    enable_interrupt();
    
    if(get_debug_timer_count())
        kprint_timers();
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
        if(pending_timers != NULL)
            next_timeout = pending_timers->time_out;
    }
    timer->flags &= ~TIMER_INUSE;
}

