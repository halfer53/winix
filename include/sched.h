#ifndef _SCHED_H_
#define _SCHED_H_

void sched_yield();

#if defined(__wramp__) & !defined(_SYSTEM)

#define sched_yield()                       wramp_syscall(SCHED_YIELD)
#endif

#endif
