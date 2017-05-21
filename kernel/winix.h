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
#include <stddef.h>
#include <type.h>
#include <util.h>
#include <size.h>
#include <stdbool.h>

#include "winix/slab.h"

#include "wramp.h"
#include "proc.h"
#include "exception.h"
#include "system.h"
#include "idle.h"
#include "sys_memory.h"
#include "exec.h"
#include "bitmap.h"
#include "message_queue.h"


#define DEBUG 0
#define HEAD 0
#define TAIL 1

//Major and minor version numbers for WINIX.
#define MAJOR_VERSION 2
#define MINOR_VERSION 0

#define align1k(x) (((((x)-1)>>10)<<10)+1024)

#define is_addr_in_same_page(a,b)	((((int)a)/1024) == (((int)b)/1024))
#define is_addr_in_consecutive_page(a,b)	((((int)a)/1024) == ((((int)b)/1024)-1))

#define get_physical_addr(va,proc)	(((int *)va)+(int)proc->rbase)
#define get_virtual_addr(pa,proc)	(((int *)pa)-(int)proc->rbase)

#define get_page_index(pa)	( (align1k((int)pa)) / 1024 )
#define physical_len_to_page_len(pa_len)	( (align1k((int)pa_len)) / 1024 )
#define page_len_to_physical_len(p_len)	(p_len * 1024)

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

// //TODO: remove these prototypes
// void rocks_main();
// void serial_main();
// void parallel_main();
// void shell_main();

#endif
