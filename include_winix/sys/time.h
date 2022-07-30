#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include <sys/types.h>

#define ITIMER_REAL         0
#define ITIMER_VIRTUAL      1
#define ITIMER_PROF         2

struct timeval {
    time_t      tv_sec;         /* seconds */
    suseconds_t tv_usec;        /* microseconds */
};

struct itimerval {
    struct timeval it_interval; /* Interval for periodic timer */
    struct timeval it_value;    /* Time until next expiration */
};

int setitimer(int which, const struct itimerval * new_value,struct itimerval * old_value);

#if defined(__wramp__) & !defined(LINTING) && !defined(_SYSTEM)

#define setitimer(which, new_value, old_value) \
    wramp_syscall(SETITIMER, which, new_value, old_value)

#endif

#endif
