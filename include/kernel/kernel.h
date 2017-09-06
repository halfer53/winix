/**
 * 
 * commond kernel header
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:25:03
 * 
*/
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
#include <debug.h>

#include <winix/type.h>
#include <winix/page.h>
#include <winix/comm.h>
#include <winix/util.h>
#include <winix/kdebug.h>
#include <winix/bitmap.h>
#include <winix/mm.h>
#include <winix/sys_stdio.h>
#include <winix/wini_ipc.h>
#include <kernel/clock.h>
#include <winix/timer.h>
#include <kernel/proc.h>
#include <kernel/sched.h>

#include <winix/syscall_ctx.h>
#include <winix/rex.h>

#define FREE_MEM_END    0x1ffff

//Memory limits
extern unsigned int TEXT_BEGIN, DATA_BEGIN, BSS_BEGIN;
extern unsigned int TEXT_END, DATA_END, BSS_END;

#endif
