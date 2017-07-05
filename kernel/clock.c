#include "winix.h"

void deliver_alarm(pid_t pid, clock_t time){
    send_signal(get_proc(pid),SIGALRM);
}

void clock_handler(){
    timer_t *timer;
    system_uptime++;

    if(pending_timers != NULL && pending_timers->timer == system_uptime){
        timer = dequeue_alarm();
        timer->handler(timer->pid,timer->timer);
    }
}
