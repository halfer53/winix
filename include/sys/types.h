#ifndef _TYPE_H_
#define _TYPE_H_ 1


#ifndef _SIZE_T
#define	_SIZE_T
typedef unsigned int	size_t;		/* type returned by sizeof */
#endif /* _SIZE_T */

typedef unsigned int char32_t;
typedef unsigned char byte;
typedef unsigned char byte_t;
typedef unsigned int uint32_t;
typedef unsigned int clock_t;
typedef unsigned int mode_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef unsigned int int32_t;
typedef unsigned int wchar_t;

# ifndef __off_t_defined
typedef unsigned int off_t;
# define __off_t_defined
#endif

typedef unsigned int time_t;
typedef unsigned int nlink_t;
typedef int pid_t;
typedef unsigned int blksize_t;
typedef unsigned int blkcnt_t;
typedef unsigned int zone_t;
typedef int ino_t;
typedef unsigned int block_t;
typedef unsigned int sector_t;
typedef unsigned int dev_t;       /* holds (major|minor) device pair */
typedef unsigned int bitchunk_t; /* collection of bits in a bitmap */

#endif
