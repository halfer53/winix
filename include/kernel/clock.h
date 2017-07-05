#ifndef _W_CLOCK_H_
#define _W_CLOCK_H_

#define _NTIMERS	10

typedef struct alarm{
	proc_t *who;
	clock_t timer;
	struct alarm *next;
}timer_t;


extern timer_t *pending_alarm[2];
extern timer_t *free_alarm[2];


void clock_handler();
void init_alarm();
void enqueue_alarm(timer_t **q, timer_t *new_alarm);
timer_t* dequeue_alarm(timer_t **q);
void sys_alarm(proc_t *who, message_t *m);

// extern timer_t timer_table[_NTIMERS];
// extern timer_t *pending_alarm[2];
// extern timer_t *free_alarm[2];
#endif


