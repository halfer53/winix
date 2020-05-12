#ifndef _FS_FS_H_
#define _FS_FS_H_ 1

#ifndef _SYSTEM
#define _SYSTEM
#endif

#include <stddef.h>
#include <curses.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/dirent.h>
#include <sys/unistd.h>
#include <fs/type.h>
#include <fs/const.h>
#include <winix/type.h>
#include <fs/inode.h>
#include <fs/filp.h>
#include <kernel/proc.h>
#include <fs/cache.h>
#include <fs/dev.h>
#include <fs/path.h>
#include <fs/super.h>
#include <winix/bitmap.h>
#include <winix/page.h>
#include <winix/compiler.h>
#include <winix/mm.h>
#include <stdbool.h>
#include <winix/kstring.h>
#include <fs/fs_methods.h>

#ifdef FILE_SYSTEM_PROJECT

#include <string.h>
#include <stdio.h>
#include "excluded_files/cmake_util.h"
#define kprintf(...) printf(__VA_ARGS__)
#ifdef _DEBUG
    #define KDEBUG(token)   \
    do{\
    printf("[SYSTEM] "); \
    printf token; \
    }while(0)
#else
    #define KDEBUG(token)
#endif

#else // else not FILE_SYSTEM_PROJECT

#include <winix/sys_stdio.h>
#include <winix/slab.h>
#include <winix/kdebug.h>
#include <string.h>

#endif // FILE_SYSTEM_PROJECT

#endif

