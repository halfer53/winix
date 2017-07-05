#ifndef _W_TIMER_H_
#define _W_TIMER_H_ 1

typedef void (*timerhandler_t)(pid_t,clock_t);

typedef struct timer{
	pid_t pid;
	clock_t timer;
	struct timer *next;
	timerhandler_t handler;
}timer_t;

extern timer_t *pending_timers;

void enqueue_alarm(timer_t *new_alarm);
timer_t* dequeue_alarm();

#endif


