#include "../winix.h"

/**
 * Syscall in this file: alarm
 * Input:   i1: seconds
 *
 * Return: i1: previous timer seconds
 * 
 */

//alarm syscall
//input     m.i1    seconds
//output    m.i1    previous timeout
int do_alarm(struct proc *who, struct message *m){
    clock_t seconds;
    struct timer *alarm;
    clock_t prev_timeout;

    if(m->i1 < 0)
        return EINVAL;

    seconds = (clock_t )m->i1; 
    alarm = &who->alarm;
    prev_timeout = alarm->time_out; //return previous alarm

    if(alarm->flags & TIMER_INUSE){
        remove_timer(alarm);
        // kprintf("remove timer\n");
    }
    
    //if seconds is 0, any pending alarm is canceled
    if(seconds != 0){
        alarm->proc_nr = who->proc_nr;
        alarm->time_out = system_uptime + seconds * 60;
        alarm->handler = &deliver_alarm;
        insert_timer(alarm);
    }
    return prev_timeout;
}
