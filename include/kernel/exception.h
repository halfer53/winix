/**
 * Exception-handling routines for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 *  2016-11-20		Bruce Tan			Modified
 **/

#ifndef _W_EXCEPTION_H_
#define _W_EXCEPTION_H_

//System uptime, in ticks.
extern int system_uptime;

/**
 * Initialises Exception Handlers and configures timer.
 **/
void init_exceptions();

#endif
