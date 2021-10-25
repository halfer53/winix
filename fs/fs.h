#ifndef _FS_FS_H_
#define _FS_FS_H_ 1

#include <kernel/kernel.h>

#include <sys/tty.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/dirent.h>

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

#ifdef FSUTIL

#include <stdio.h>
#include "fsutil/fsutil.h"

#else // else not FSUTIL

#include <sys/unistd.h>
#include <winix/sys_stdio.h>
#include <winix/slab.h>
#include <winix/kdebug.h>

#endif // FSUTIL

#endif

