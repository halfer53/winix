#ifndef _W_CLOCK_H_
#define _W_CLOCK_H_

#define _NTIMERS	10

typedef struct alarm{
	proc_t *who;
	clock_t timer;
	struct alarm *next;
}alarm_t;

void clock_handler();
void init_alarm();
void sys_alarm(proc_t *caller, clock_t time);

// extern alarm_t alarm_table[_NTIMERS];
// extern alarm_t *pending_alarm[2];
// extern alarm_t *free_alarm[2];
#endif


