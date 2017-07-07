#ifndef _W_KERNEL_H_
#define _W_KERNEL_H_ 1

#define _SYSTEM 1

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1
#endif

#ifdef _GCC_DEBUG
#define kprintf(...) printf(__VA_ARGS__)
#endif

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stddef.h>
#include <util.h>
#include <const.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <stdbool.h>
#include <curses.h>

#include <winix/type.h>
#include <winix/comm.h>
#include <winix/kdebug.h>
#include <winix/bitmap.h>
#include <winix/mm.h>
#include <winix/sys_stdio.h>
#include <winix/wini_ipc.h>
#include <winix/timer.h>
#include <winix/proc.h>

#include <winix/syscall_ctx.h>

#endif
