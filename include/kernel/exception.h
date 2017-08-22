/**
 * Exception-handling routines for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#ifndef _K_EXCEPTION_H_
#define _K_EXCEPTION_H_ 1

//System uptime, in ticks.
extern int system_uptime;

/**
 * Initialises Exception Handlers and configures timer.
 **/
void init_exceptions();
int irq_count();
int* get_exception_top();
int* get_exception_bottom();

#define in_interrupt()  (irq_count())
#define EXCEPTION_STACK_SIZE    300

#endif
