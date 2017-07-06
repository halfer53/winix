#include "winix.h"

void deliver_alarm(pid_t pid, clock_t time){
    send_sig(get_proc(pid),SIGALRM);
}

void clock_handler(){
    timer_t *timer;

    timer = dequeue_alarm();
    if(timer != NULL && timer->time_out == system_uptime)
        timer->handler(timer->pid,timer->time_out);
    else
        kprintf("Inconsis alarm %d %d from %d\n",system_uptime,next_timeout,timer->pid);

}
