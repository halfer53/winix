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
#include <winix/rex.h>

// System uptime, stored as number of timer interrupts since boot
PRIVATE clock_t system_uptime = 0;

// Global variable for the next timeout event
PUBLIC clock_t next_timeout = 0;

PRIVATE struct proc* bill_ptr;

void do_ticks();

// void clock_main(){
//     struct message m;
//     while(1){
//         winix_receive(&m);
//         switch(m.type){
//             case DO_CLOCKTICK:
//                 do_ticks();
//                 break;
//             default:
//                 kerror("CLOCK received %d from %d\n",m.type,m.src);
//         }
//     }
// }

void do_ticks(){
    while(next_timeout <= system_uptime){
        struct timer* next_timer = dequeue_alarm();
        if(next_timer){
            next_timer->handler(next_timer->proc_nr,next_timer->time_out);
        }
    }
}

void set_bill_ptr(struct proc* who){
    bill_ptr = who;
}

clock_t get_uptime(){
    return system_uptime;
}

/**
 * Timer (IRQ2)
 *
 * NOTE: this method is called during exception context
 * 
 * This method is called for every timer interrupt
 * 
 * Side Effects:
 *   system_uptime is incremented
 *   if there is an immediate timer, relevant handler is called
 *   scheduler is called (i.e. this handler does not return)
 **/
void clock_handler(){
    RexTimer->Iack = 0;

    // Increment uptime, and check if there is any alarm
    system_uptime++;

    // Accounting
    curr_scheduling_proc->time_used++;
    curr_scheduling_proc->ticks_left--;

    if(curr_scheduling_proc->flags & BILLABLE){
        bill_ptr->sys_time_used++;
    }
    do_ticks();
    sched();
}
