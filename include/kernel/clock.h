#ifndef _K_CLOCK_H_
#define _K_CLOCK_H_ 1

void clock_handler();
void sys_alarm(proc_t *who, message_t *m);
extern void deliver_alarm(pid_t pid, clock_t time);

#endif
