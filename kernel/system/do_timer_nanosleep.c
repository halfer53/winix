/**
 * Syscall in this file: alarm, setitimer, nanosleep
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
#include <time.h>

void deliver_alarm(int proc_nr, clock_t time){
    struct proc* who = get_non_zombie_proc(proc_nr);
    if(who){
        send_sig(who, SIGALRM);
        if(who->state){
            handle_pendingsig(who, true);
        }
        if(who->state == STATE_RUNNABLE && who->timer_interval){
            new_timer(proc_nr, &who->timer, who->timer_interval, deliver_alarm);
        }
    }
}

clock_t convert_timeval_to_hz(const struct timeval *tv){
    clock_t hz, micro_hz;
    hz = tv->tv_sec * HZ;
    micro_hz = tv->tv_usec / (1000000 / HZ);
    if (tv->tv_usec && micro_hz == 0)
        micro_hz = 1;
    return hz + micro_hz;
}

clock_t convert_timespec_to_hz(const struct timespec *tv){
    clock_t hz, nano_hz;
    hz = tv->tv_sec * HZ;
    nano_hz = tv->tv_nsec / (1000000000 / HZ);
    if (tv->tv_nsec && nano_hz == 0)
        nano_hz = 1;
    return hz + nano_hz;
}


int sys_setitimer(struct proc* who, int which, const struct itimerval* new_value, struct itimerval* old_value){
    struct timer *timer;
    clock_t prev_timeout;
    clock_t new_timeout, interval;
    int microseconds = (1000 * 1000);
    int micro_seconds_period = microseconds / HZ;

    if (which != ITIMER_REAL)
        return -EINVAL;

    timer = &who->timer;
    prev_timeout = timer->time_out; 
    who->timer_interval = 0;

    if(timer->flags & TIMER_INUSE){
        remove_timer(timer);
    }

    new_timeout = convert_timeval_to_hz(&new_value->it_value);
    interval = convert_timeval_to_hz(&new_value->it_interval);
    who->timer_interval = interval;

    if(new_timeout > 0){
        new_timer(who->proc_nr, timer, new_timeout, deliver_alarm);
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

    if(!is_vaddr_ok((vptr_t *)act, sizeof(struct itimerval), who))
        return -EFAULT;

    if(oact && !is_vaddr_ok((vptr_t *)oact, sizeof(struct itimerval), who))
        return -EFAULT;

    act = (struct itimerval*)get_physical_addr(act, who);
    if (oact){
        oact = (struct itimerval*)get_physical_addr(oact, who);
    }
    return sys_setitimer(who, m->m1_i1, act, oact);
}


int do_alarm(struct proc *who, struct message *m){
    int ret;
    struct itimerval act, oact;
    memset(&act, 0, sizeof(struct itimerval));
    act.it_value.tv_sec = m->m1_i1;
    ret = sys_setitimer(who, ITIMER_REAL, &act, &oact);
    if (ret)
        return ret;
    return oact.it_value.tv_sec;
}

void _wakeup_process(int proc_nr, clock_t time){
    struct proc* p;
    struct message _message;
    p = get_proc(proc_nr);
    if(p){
        syscall_reply2(NANOSLEEP, 0, proc_nr, &_message);
    }
}

int do_nanosleep(struct proc* who, struct message* m){
    clock_t ticks;
    int ret;
    struct timer *alarm;
    struct timespec* req;

    if(!is_vaddr_ok(m->m1_p1, sizeof(struct timespec), who))
        return -EFAULT;

    req = (struct timespec*)get_physical_addr(m->m1_p1, who);

    alarm = &who->timer;
    if(alarm->flags & TIMER_INUSE){
        return -EINVAL;
    }

    ticks = convert_timespec_to_hz(req);
    
    ret = new_timer(who->proc_nr, alarm, ticks, _wakeup_process);
    if (ret)    
        return ret;

    return SUSPEND;
}
