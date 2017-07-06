#include "../winix.h"

void syscall_alarm(proc_t *who, message_t *m){
    clock_t seconds;
    timer_t *alarm;

    seconds = (clock_t )m->i1;
    alarm = &who->alarm;
    m->i1 = alarm->time_out; //return previous alarm

    if(alarm->flags & TIMER_INUSE){
        remove_timer(alarm);
        // kprintf("remove timer\n");
    }
    
    //if seconds is 0, any pending alarm is canceled
    if(seconds != 0){
        alarm->pid = who->pid;
        alarm->time_out = system_uptime + seconds * 60;
        alarm->handler = &deliver_alarm;
        insert_timer(alarm);
    }
    winix_send(who->pid, m);
}
