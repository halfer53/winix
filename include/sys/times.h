/* The <times.h> header is for time times() system call. */

#ifndef _TIMES_H_
#define _TIMES_H_

#include <sys/types.h>

struct tms {
  clock_t tms_utime;
  clock_t tms_stime;
  clock_t tms_cutime;
  clock_t tms_cstime;
};

#endif /* _TIMES_H */

