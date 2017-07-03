/**
 * Globals used in WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#ifndef _WINIX_H_
#define _WINIX_H_

#include <winix/kernel.h>
#include <ucontext.h>

#include <winix/exec.h>
#include <winix/slab.h>
#include <winix/signal.h>

#include <kernel/exception.h>
#include <kernel/system.h>
#include <kernel/idle.h>
#include <kernel/sys_memory.h>
#include <kernel/clock.h>


//Major and minor version numbers for WINIX.
#define MAJOR_VERSION 2
#define MINOR_VERSION 0
// extern int DEBUG = 0;
/**
 * Print an error message and lock up the OS... the "Blue Screen of Death"
 *
 * Side Effects:
 *   OS locks up.
 **/
void panic(const char *message);

/**
 * Asserts that a condition is true.
 * If so, this function has no effect.
 * If not, panic is called with the appropriate message.
 */
void assert(int expression, const char *message);

extern message_t *curr_mesg();
extern void intr_syscall();
extern ucontext_t syscall_ctx;
extern ucontext_t recv_ctx;

//Memory limits
extern unsigned long TEXT_BEGIN, DATA_BEGIN, BSS_BEGIN;
extern unsigned long TEXT_END, DATA_END, BSS_END;
extern unsigned long FREE_MEM_BEGIN, FREE_MEM_END; //calculated at runtime

// //TODO: remove these prototypes
// void rocks_main();
// void serial_main();
// void parallel_main();
// void shell_main();

#endif

