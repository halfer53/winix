#ifndef _TIME_H_
#define _TIME_H_

#include <sys/times.h>

clock_t times(struct tms *buf);
#if defined(__wramp__) & !defined(_SYSTEM)
#define times(buf)                          wramp_syscall(TIMES, buf)
#endif

#endif
