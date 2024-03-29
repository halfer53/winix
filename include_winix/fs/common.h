#ifndef _FS_CONST_HEAD_
#define _FS_CONST_HEAD_

#include <stddef.h>
#include <fs/type.h>
#include <sys/macro.h>
#include <sys/arch.h>
#include <winix/page.h>

#define ROOT_INODE_NUM          1
#define BLOCK_SIZE_DWORD        256
#define BLOCK_SIZE              (BLOCK_SIZE_DWORD * DWORD_SIZE)

#define DISK_PAGE_NR            (PAGE_LEN / BLOCK_SIZE * 40)
#define DISK_SIZE               (DISK_PAGE_NR * BLOCK_SIZE)
#define DISK_SIZE_DWORD         (DISK_PAGE_NR * BLOCK_SIZE_DWORD)

#define INODE_NUM   496
#define NR_TZONES   8

#define NR_FILPS          32    /* # slots in filp table */
#define NR_INODES         48    /* # slots in "in core" inode table */
#define NR_SUPERS          8    /* # slots in super block table */
#define NR_LOCKS           8    /* # slots in the file locking table */

#define READING 1
#define WRITING 2

#ifndef PRIVATE
#define PRIVATE static
#endif

#define VERIFY_READ     1
#define VERIFY_WRITE    2
#define ROOT_DEV    (0x0101)    /* MAKEDEV(1,1) */

#define SUPER_BLOCK_MAGIC   (0xabcdefff)

extern unsigned int start_unix_time;
#define get_unix_time()         (start_unix_time + (get_uptime() / HZ))

extern char DISK_RAW[DISK_SIZE];

#endif

