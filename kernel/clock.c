/**
 * 
 * Winix clock exception handler
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/

#include "winix.h"

//System uptime, stored as number of timer interrupts since boot
PRIVATE clock_t system_uptime = 0;

//Global variable for the next timeout event
PUBLIC clock_t next_timeout = 0;

PRIVATE struct proc* bill_ptr;

void set_bill_ptr(struct proc* who){
    bill_ptr = who;
}

void deliver_alarm(int proc_nr, clock_t time){
    cause_sig(get_proc(proc_nr),SIGALRM);
}

void handle_timer(struct timer *timer){

    if(timer != NULL && timer->time_out == get_uptime())
        timer->handler(timer->proc_nr,timer->time_out);
    else
        kprintf("Inconsis alarm %d %d from %d\n",get_uptime(),next_timeout,timer->proc_nr);
}

clock_t get_uptime(){
    return system_uptime;
}

/**
 * Timer (IRQ2)
 *
 * Side Effects:
 *   system_uptime is incremented
 *   if there is an immediate timer, relevant handler is called
 *   scheduler is called (i.e. this handler does not return)
 **/
void clock_handler(){
    RexTimer->Iack = 0;

    //Increment uptime, and check if there is any alarm
    system_uptime++;
        
    while(next_timeout == system_uptime){
        handle_timer(dequeue_alarm());
    }

    //Accounting
    current_proc->time_used++;
    current_proc->ticks_left--;

    if(current_proc->i_flags & BILLABLE){
        bill_ptr->sys_time_used++;
    }

    sched();
}
