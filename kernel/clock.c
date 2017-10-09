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

#include <kernel/kernel.h>
#include <kernel/clock.h>
#include <kernel/table.h>
#include <kernel/exception.h>

//System uptime, stored as number of timer interrupts since boot
PRIVATE clock_t system_uptime = 0;

//Global variable for the next timeout event
PUBLIC clock_t next_timeout = 0;

PRIVATE struct proc* bill_ptr;

void do_ticks();

void clock_main(){
    struct message m;
    while(1){
        winix_receive(&m);
        switch(m.type){
            case DO_CLOCKTICK:
                do_ticks();
                break;
            default:
                kerror("CLOCK received %d from %d\n",m.type,m.src);
        }
    }
}

bool has_next_timeout(){
    return next_timeout <= system_uptime;
}

void do_ticks(){
    while(has_next_timeout()){
        struct timer* next_timer = dequeue_alarm();
        next_timer->handler(next_timer->proc_nr,next_timer->time_out);
    }
}

void set_bill_ptr(struct proc* who){
    bill_ptr = who;
}

void deliver_alarm(int proc_nr, clock_t time){
    send_sig(get_proc(proc_nr),SIGALRM);
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

    //Accounting
    current_proc->time_used++;
    current_proc->ticks_left--;

    if(current_proc->i_flags & BILLABLE){
        bill_ptr->sys_time_used++;
    }

    if(next_timeout <= system_uptime){
        struct message* m = get_exception_m();
        m->type = DO_CLOCKTICK;
        do_notify(INTERRUPT, CLOCK, m);
    }
    sched();
}
