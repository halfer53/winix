/* The <times.h> header is for time times() system call. */

#ifndef _TIMES_H_
#define _TIMES_H_

typedef unsigned int clock_t;

#ifndef __time_t_defined
typedef unsigned int time_t;
#define __time_t_defined
#endif

struct tms {
  clock_t tms_utime;
  clock_t tms_stime;
  clock_t tms_cutime;
  clock_t tms_cstime;
};

#endif /* _TIMES_H */

