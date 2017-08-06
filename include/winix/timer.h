#ifndef _W_TIMER_H_
#define _W_TIMER_H_ 1

typedef void (*timerhandler_t)(pid_t,clock_t);

#define TIMER_INUSE	1

struct timer{
	pid_t pid;
	clock_t time_out;
	struct timer *next;
	timerhandler_t handler;
	int flags;
};

extern clock_t next_timeout;

void insert_timer(struct timer *timer);
struct timer* dequeue_alarm();
void remove_timer(struct timer *timer);

#endif


