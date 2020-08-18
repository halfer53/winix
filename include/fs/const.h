#ifndef _FS_CONST_HEAD_
#define _FS_CONST_HEAD_

#include <sys/types.h>
#include <fs/type.h>

#define ROOT_INODE_NUM  1

#define BLOCK_SIZE_WORD  (1024)

#ifdef __x86_64__
#define SECTOR_SIZE 512
#define BLOCK_SIZE  4096
#define TO_WORD_SIZE(x) (x / 4)
#define TO_CHAR_SIZE(x) (x * 4)
#else
#define SECTOR_SIZE 80
#define BLOCK_SIZE  1024
#define TO_WORD_SIZE(x) (x)
#define TO_CHAR_SIZE(x) (x)
#endif

#define INODE_NUM   496
#define NR_TZONES   8

#define NR_FILPS          32    /* # slots in filp table */
#define NR_INODES         48    /* # slots in "in core" inode table */
#define NR_SUPERS          8    /* # slots in super block table */
#define NR_LOCKS           8    /* # slots in the file locking table */

#define MAKEDEV(dmajor, dminor) ((((unsigned int)dmajor << 8) & 0xFF00U) | ((unsigned int)dminor & 0xFFFF00FFU))
#define DEV_MAJOR(devnum)           (((unsigned int)devnum & 0xFF00U) >> 8)
#define DEV_MINOR(devnum)           ((unsigned int)devnum & 0xFFFF00FFU)
#define TTY_DEV_NUM     (0x301)
#define TTY2_DEV_NUM     (0x302)

#define READING 1
#define WRITING 2

#ifndef PRIVATE
#define PRIVATE static
#endif

#define VERIFY_READ     1
#define VERIFY_WRITE    2
#define ROOT_DEV    (0x0101)    /* MAKEDEV(1,1) */

#define SUPER_BLOCK_MAGIC   (0xabcdefff)

#define DISK_PAGE_NR    (40)
#define DISK_SIZE   (DISK_PAGE_NR * BLOCK_SIZE)
#define DISK_SIZE_WORD   (DISK_PAGE_NR * BLOCK_SIZE_WORD)

extern char DISK_RAW[DISK_SIZE];

#endif

