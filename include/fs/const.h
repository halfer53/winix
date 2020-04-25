#ifndef _FS_CONST_HEAD_
#define _FS_CONST_HEAD_

#define ROOT_INODE_NUM  1


#define SECTOR_SIZE 512
#define BLOCK_SIZE  1024

#define INODE_NUM   496
#define NR_TZONES   8

#define NR_FILPS          64    /* # slots in filp table */
#define NR_INODES         64    /* # slots in "in core" inode table */
#define NR_SUPERS          8    /* # slots in super block table */
#define NR_LOCKS           8    /* # slots in the file locking table */

#define MAKEDEV(dmajor, dminor) ((((unsigned int)dmajor << 8) & 0xFF00U) | ((unsigned int)dminor & 0xFFFF00FFU))
#define DEV_MAJOR(devnum)           (((unsigned int)devnum & 0xFF00U) >> 8)
#define DEV_MINOR(devnum)           ((unsigned int)devnum & 0xFFFF00FFU)

#define READING 1
#define WRITING 2

#ifndef PRIVATE
#define PRIVATE static
#endif

#endif

