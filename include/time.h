#ifndef _TIME_H_
#define _TIME_H_

#include <sys/times.h>

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

clock_t times(struct tms *buf);
int nanosleep(const struct timespec *req, struct timespec *rem);


#if defined(__wramp__) & !defined(LINTING) && !defined(_SYSTEM)

#define times(buf)                          wramp_syscall(TIMES, buf)
#define nanosleep(req, rem)                 wramp_syscall(NANOSLEEP, req, rem)

#endif

#endif
