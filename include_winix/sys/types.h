#ifndef _TYPE_H_
#define _TYPE_H_ 1

#include <stdint.h>

typedef int pid_t;
typedef unsigned int mode_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef unsigned int nlink_t;
typedef unsigned int blksize_t;
typedef unsigned int blkcnt_t;
typedef unsigned int zone_t;
typedef int ino_t;
typedef unsigned int block_t;
typedef unsigned int sector_t;
typedef unsigned int dev_t;       /* holds (major|minor) device pair */
typedef unsigned int bitchunk_t; /* collection of bits in a bitmap */

# ifndef __off_t_defined
typedef unsigned int off_t;
# define __off_t_defined
#endif

#ifndef __time_t_defined
typedef unsigned int time_t;
#define __time_t_defined
#endif

#ifndef __suseconds_t_defined
typedef unsigned int suseconds_t;
#define __suseconds_t_defined
#endif

#endif
