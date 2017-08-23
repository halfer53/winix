/**
 * 
 * Winix timer utilities
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:42
 * @modify date 2017-08-23 06:22:42
*/
#ifndef _W_TIMER_H_
#define _W_TIMER_H_ 1

typedef void (*timerhandler_t)(int,clock_t);

#define TIMER_INUSE	1

struct timer{
	int proc_nr;
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


