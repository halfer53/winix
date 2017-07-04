#include "../winix.h"

void sys_alarm(proc_t *who, clock_t time){
    alarm_t *new_alarm;

    new_alarm = dequeue_alarm(free_alarm);
    new_alarm->timer = system_uptime + time * 60;
    new_alarm->who = who;

    enqueue_alarm(pending_alarm,new_alarm);
}
