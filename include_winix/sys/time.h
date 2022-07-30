#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

#include <sys/types.h>

struct timeval {
    time_t      tv_sec;         /* seconds */
    suseconds_t tv_usec;        /* microseconds */
};

struct itimerval {
    struct timeval it_interval; /* Interval for periodic timer */
    struct timeval it_value;    /* Time until next expiration */
};



#endif
