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
#include <sys/unistd.h>
#include <stddef.h>
#include <util.h>
#include <const.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <stdbool.h>
#include <curses.h>
#include <debug.h>
#include <string.h>
#include <stdio.h>

#include <winix/compiler.h>
#include <winix/type.h>
#include <winix/page.h>
#include <winix/comm.h>
#include <winix/util.h>
#include <winix/kdebug.h>
#include <winix/bitmap.h>
#include <winix/mm.h>
#include <winix/sys_stdio.h>
#include <winix/wini_ipc.h>
#include <winix/syscall_ctx.h>
#include <winix/rex.h>
#include <winix/exec.h>
#include <winix/mem_alloc.h>
#include <winix/signal.h>
#include <winix/timer.h>
#include <kernel/clock.h>
#include <kernel/proc.h>
#include <kernel/exception.h>
#include <kernel/system.h>
#include <kernel/table.h>

#define FREE_MEM_END    0x1ffff

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 *
 */
 #define container_of(ptr, type, member) ({                      \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

#define STR_MALLEN(x) (strlen(x) + 1)

//Memory limits
extern unsigned int TEXT_BEGIN, DATA_BEGIN, BSS_BEGIN;
extern unsigned int TEXT_END, DATA_END, BSS_END;

#endif
