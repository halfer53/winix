#include "../winix.h"

void sys_alarm(proc_t *caller, clock_t time){
    alarm_t *new_alarm;

    new_alarm = dequeue_alarm(free_alarm);
    new_alarm->timer = system_uptime + time * 58;
    new_alarm->who = caller;

    enqueue_alarm(pending_alarm,new_alarm);
    
}
