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
#include <stdbool.h>
#include <winix/kstring.h>
#include "fs_methods.h"

#ifdef FS_CMAKE

#include <string.h>
#include <stdio.h>
#include "cmake/cmake_util.h"
#define kprintf(...) printf(__VA_ARGS__)
#define KDEBUG(token)   printf("[SYSTEM] "); printf token

#else

#include <winix/slab.h>
#include <winix/kdebug.h>
#include <winix_string.h>

#endif

#endif

