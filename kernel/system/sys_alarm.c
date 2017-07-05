#include "../winix.h"

void syscall_alarm(proc_t *who, message_t *m){
    clock_t clock_time = (clock_t )m->i1;
    timer_t *alarm = &who->alarm;

    alarm->timer = system_uptime + clock_time * 60;
    alarm->pid = who->pid;
    alarm->handler = &deliver_alarm;    

    enqueue_alarm(alarm);

    m->i1 = 0;
    winix_send(who->pid, m);
    
}
