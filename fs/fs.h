#ifndef _FS_FS_H_
#define _FS_FS_H_ 1

#include <kernel/kernel.h>

#include <sys/tty.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/dirent.h>
#ifndef MAKEFS_STANDALONE
#include <sys/unistd.h>
#endif
#include <stdbool.h>

#include <fs/type.h>
#include <fs/common.h>
#include <fs/inode.h>
#include <fs/filp.h>
#include <fs/cache.h>
#include <fs/path.h>
#include <fs/super.h>
#include <fs/fs_methods.h>

#include <winix/dev.h>
#include <winix/bitmap.h>
#include <winix/compiler.h>
#include <winix/kstring.h>

#ifdef MAKEFS_STANDALONE

#include <stdio.h>
#include "makefs_only/cmake_util.h"

#else // else not MAKEFS_STANDALONE

#include <winix/sys_stdio.h>
#include <winix/slab.h>
#include <winix/kdebug.h>

#endif // MAKEFS_STANDALONE

#endif

