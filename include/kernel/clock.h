/**
 * 
 * kernel clock handler
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:24:51
 * 
*/
#ifndef _K_CLOCK_H_
#define _K_CLOCK_H_ 1

// System uptime, in ticks.
clock_t get_uptime();

extern clock_t next_timeout;

void clock_handler();
void sys_alarm(struct proc *who, struct message *m);
extern void deliver_alarm(int procnr, clock_t time);
void wakeup_process(int procnr, clock_t time);
void clock_main();

#endif
