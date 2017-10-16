#ifndef _K_SCHED_H_
#define _K_SCHED_H_ 1

#define REBALANCE_TIMEOUT   (36)

void rebalance_queues(int proc_nr, clock_t time);
void sched();
struct proc *pick_proc();
void set_bill_ptr(struct proc* who);

#endif
