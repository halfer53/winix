#ifndef _FS_CONST_H_
#define _FS_CONST_H_ 1


typedef unsigned int block_t;
#define SECTOR_SIZE 512
#define BLOCK_SIZE  1024

#define INODE_NUM   496
#define NR_TZONES   7

#define NR_FILPS          64	/* # slots in filp table */
#define NR_INODES         64	/* # slots in "in core" inode table */
#define NR_SUPERS          8	/* # slots in super block table */
#define NR_LOCKS           8	/* # slots in the file locking table */

typedef unsigned int zone_t;
typedef unsigned int ino_t;

typedef unsigned int dev_t;
typedef unsigned int size_t;
typedef unsigned int byte;
typedef unsigned int byte_t;
typedef unsigned int uint32_t;
typedef unsigned int clock_t;
typedef unsigned int mode_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef unsigned int off_t;
//typedef unsigned int time_t;
typedef unsigned int nlink_t;
typedef unsigned int block_t;
typedef unsigned int sector_t;
typedef int          dev_t;	   /* holds (major|minor) device pair */
typedef unsigned int bitchunk_t; /* collection of bits in a bitmap */

#define READING 1
#define WRITING 2

#define PRIVATE static

#endif