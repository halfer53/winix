#include "../winix.h"

void syscall_alarm(proc_t *who, message_t *m){
    clock_t clock_time = (clock_t )m->i1;
    timer_t *new_alarm;

    new_alarm = dequeue_alarm(free_alarm);
    new_alarm->timer = system_uptime + clock_time * 60;
    new_alarm->who = who;

    enqueue_alarm(pending_alarm,new_alarm);

    m->i1 = 0;
    winix_send(who->pid, m);
}
