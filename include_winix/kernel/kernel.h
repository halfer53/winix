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

#include <sys/ipc.h>
#include <sys/debug.h>
#include <sys/syscall.h>
#include <stddef.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <bsd/string.h>

#include <kernel/proc.h>
#include <kernel/system.h>
#include <kernel/exception.h>
#include <winix/const.h>
#include <winix/comm.h>
#include <winix/compiler.h>
#include <winix/type.h>
#include <winix/page.h>
#include <winix/kdebug.h>
#include <winix/sys_stdio.h>
#include <winix/wini_ipc.h>
#include <winix/mm.h>
#include <winix/slab.h>

#define FREE_MEM_END    0x1ffff

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define STR_MALLEN(x) (strlen(x) + 1)

// Memory limits
extern unsigned int TEXT_BEGIN, DATA_BEGIN, BSS_BEGIN;
extern unsigned int TEXT_END, DATA_END, BSS_END;

#ifdef FSUTIL

#include <stdio.h>
#define kprintf(...) printf(__VA_ARGS__)
#define get_free_pages(num, flag)   kmalloc(num, sizeof(char))
#define release_pages(ptr, num)     kfree(ptr)
#define klog(...)                   printf(__VA_ARGS__)
#define kwarn(...)                  \
do{ \
printf("WARN: ");       \
printf(__VA_ARGS__);    \
}while(0)


#endif

#endif
