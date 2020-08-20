#include <kernel/kernel.h>
#include <winix/list.h>
#include <kernel/clock.h>

LIST_HEAD(timers);

void wakeup_process(int proc_nr, clock_t time){
    struct proc* p;
    p = get_proc(proc_nr);
    if(p){
        enqueue_schedule(p);
    }
}

int do_csleep(struct proc* who, struct message* m){
    clock_t ticks;
    struct timer *alarm;

    ticks = m->m1_i1;

    alarm = &who->alarm;
    if(alarm->flags & TIMER_INUSE){
        return EINVAL;
    }
    
    new_timer(who->proc_nr, alarm, ticks, wakeup_process);
    return OK;
}


