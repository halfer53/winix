#include "winix.h"

void deliver_alarm(pid_t pid, clock_t time){
    cause_sig(get_proc(pid),SIGALRM);
}

void handler_timer(timer_t *timer){

    if(timer != NULL && timer->time_out == system_uptime)
        timer->handler(timer->pid,timer->time_out);
    else
        kprintf("Inconsis alarm %d %d from %d\n",system_uptime,next_timeout,timer->pid);
}

/**
 * Timer (IRQ2)
 *
 * Side Effects:
 *   system_uptime is incremented
 *   scheduler is called (i.e. this handler does not return)
 **/
void clock_handler(){
    RexTimer->Iack = 0;

	//Increment uptime, and check if there is any alarm
	system_uptime++;
	if(next_timeout == system_uptime)
		handler_timer(dequeue_alarm());

	sched();
}
