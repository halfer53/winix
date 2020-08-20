#include <kernel/kernel.h>
#include <winix/list.h>
#include <kernel/clock.h>

struct message m;

void wakeup_process(int proc_nr, clock_t time){
    struct proc* p;
    p = get_proc(proc_nr);
    if(p){
        if(p->state & STATE_ZOMBIE)
            return;
        syscall_reply2(CSLEEP, 0, proc_nr, &m);
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
    return SUSPEND;
}


