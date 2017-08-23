/**
 * 
 * kernel clock handler
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * @create date 2017-08-23 06:24:51
 * @modify date 2017-08-23 06:24:51
*/
#ifndef _K_CLOCK_H_
#define _K_CLOCK_H_ 1

void clock_handler();
void sys_alarm(struct proc *who, struct message *m);
extern void deliver_alarm(pid_t pid, clock_t time);

#endif
