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
#include <winix/rex.h>
#include <kernel/clock.h>
#include <kernel/exception.h>


// a linked list of pending timers to be alarmed
PRIVATE struct timer *pending_timers = NULL;

void kreport_timers(){
    struct timer *mq = pending_timers;
    
    while(mq != NULL)
    {
        klog("timer timeout %d from %d\n",mq->time_out, mq->proc_nr);
        mq = mq->next;
    }
    klog("next timeout %d\n",next_timeout);
}

int new_timer(int procnr_from, struct timer* curr, clock_t timeout, timerhandler_t watchdog){

    if(timeout <= 0)
        return ERR;

    if( !(curr->flags & TIMER_INUSE) ){
        curr->flags |= TIMER_INUSE;
        curr->time_out = get_uptime() + timeout;
        curr->handler = watchdog;
        curr->proc_nr = procnr_from;
        curr->next = NULL;
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
    if(!mq)
        PANIC("No alarm in the queue");
    
    pending_timers = mq->next;
    if(pending_timers)
        next_timeout = pending_timers->time_out;
    else
        next_timeout = TIMER_NEVER;
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
    struct timer *curr;
    clock_t new_timeout;

    disable_interrupt();
    curr = pending_timers;
    new_timeout = timer->time_out;
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
    
    // if(is_debugging_timer())
    //     kreport_timers();
}

/**
 * remove timer from the pending_timers
 * @param timer 
 */
void remove_timer(struct timer *timer){
    struct timer *prev = NULL;
    struct timer *curr;

    disable_interrupt();
    curr = pending_timers;
    while(curr && curr != timer){
        prev = curr;
        curr = curr->next;
    }

    if(prev){
        prev->next = curr->next;
    }else{
        pending_timers = pending_timers->next;
        if(pending_timers != NULL)
            next_timeout = pending_timers->time_out;
    }
    timer->flags &= ~TIMER_INUSE;
    enable_interrupt();
}

