/**
 * Syscall in this file: alarm
 * Input:   m1_i1: seconds
 *
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:08:30
 * 
*/
#include <kernel/kernel.h>
#include <kernel/clock.h>
#include <winix/ksignal.h>
#include <sys/time.h>

void deliver_alarm(int proc_nr, clock_t time){
    struct proc* who = get_proc(proc_nr);
    if(who){
        send_sig(who, SIGALRM);
        if(who->state){
            handle_pendingsig(who, true);
        }
    }
}

// alarm syscall
// input     m.m1_i1    seconds
// output    m.reply_res    previous timeout
int do_alarm(struct proc *who, struct message *m){
    clock_t seconds;
    struct timer *alarm;
    clock_t prev_timeout;

    if(m->m1_i1 < 0)
        return -EINVAL;

    seconds = (clock_t )m->m1_i1; 
    alarm = &who->alarm;
    prev_timeout = alarm->time_out; // return previous alarm

    if(alarm->flags & TIMER_INUSE){
        remove_timer(alarm);
    }
    
    // if seconds is 0, any pending alarm is canceled
    if(seconds > 0){
        new_timer(who->proc_nr, alarm, seconds * HZ, deliver_alarm);
    }

    return prev_timeout;
}

int sys_setitimer(struct proc* who, int which, const struct itimerval* new_value, struct itimerval* old_value){
    struct timer *alarm;
    clock_t prev_timeout;
    clock_t new_timeout, micro_timeout;
    int microseconds = (1000 * 1000);
    int micro_seconds_period = microseconds / HZ;

    if (which != ITIMER_REAL)
        return -EINVAL;

    alarm = &who->alarm;
    prev_timeout = alarm->time_out; // return previous alarm

    if(alarm->flags & TIMER_INUSE){
        remove_timer(alarm);
    }

    new_timeout = new_value->it_value.tv_sec * HZ;
    micro_timeout = new_value->it_value.tv_usec / micro_seconds_period;
    if (micro_timeout == 0 && new_value->it_value.tv_usec != 0)
        micro_timeout = 1;
    new_timeout += micro_timeout;

    if(new_timeout > 0){
        new_timer(who->proc_nr, alarm, new_timeout, deliver_alarm);
    }

    if (old_value){
        int seconds = prev_timeout / HZ;
        int remainder = prev_timeout % HZ;
        int microseconds = remainder * micro_seconds_period;
        old_value->it_value.tv_sec = seconds;
        old_value->it_value.tv_usec = microseconds;
    }

    return 0;
}

int do_setitimer(struct proc *who, struct message *m){
    struct itimerval* act = m->m1_p1;
    struct itimerval* oact = m->m1_p2;

    if(!is_vaddr_accessible(act, who))
        return -EFAULT;

    if(oact && !is_vaddr_accessible(oact, who))
        return -EFAULT;

    act = (struct itimerval*)get_physical_addr(act, who);
    if (oact){
        oact = (struct itimerval*)get_physical_addr(oact, who);
    }
    return sys_setitimer(who, m->m1_i1, act, oact);
}
