/**
 * WRAMP-specific functions for WINIX.
 *
 * Revision History:
 *  2016-09-19		Paul Monigatti			Original
 **/

#ifndef _WRAMP_H_
#define _WRAMP_H_

#define NUM_REGS 13

/**
 * Sets the global exception handler function.
 *
 * Parameters:
 *   handler		The handler that will be called to handle exceptions.
 **/
void wramp_set_handler(void (*handler)(int estat));

/**
 * Loads the context of current_proc.
 *
 * Side Effects:
 *   This function does not return.
 **/
void wramp_load_context();

#endif
