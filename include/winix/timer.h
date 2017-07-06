#ifndef _W_TIMER_H_
#define _W_TIMER_H_ 1

typedef void (*timerhandler_t)(pid_t,clock_t);

#define TIMER_INUSE	1

typedef struct timer{
	pid_t pid;
	clock_t time_out;
	struct timer *next;
	timerhandler_t handler;
	int flags;
}timer_t;

extern clock_t next_timeout;

void insert_timer(timer_t *timer);
timer_t* dequeue_alarm();
void remove_timer(timer_t *timer);

#endif


