/**
 * Globals used in WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#ifndef _WINIX_H_
#define _WINIX_H_

#include <sys/rex.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stddef.h>

#include "../kernel/wramp.h"
#include "../kernel/proc.h"
#include "../kernel/exception.h"
#include "../kernel/system.h"
#include "../kernel/idle.h"

//Major and minor version numbers for WINIX.
#define MAJOR_VERSION 1
#define MINOR_VERSION 0

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

//Memory limits
extern unsigned long TEXT_BEGIN, DATA_BEGIN, BSS_BEGIN;
extern unsigned long TEXT_END, DATA_END, BSS_END;
extern unsigned long FREE_MEM_BEGIN, FREE_MEM_END; //calculated at runtime

//TODO: remove these prototypes
void rocks_main();
void serial_main();
void parallel_main();
void shell_main();

#endif
