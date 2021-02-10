/**
 * 
 * WRAMP-specific functions for WINIX.
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/


#ifndef _WRAMP_H_
#define _WRAMP_H_ 1

#define NUM_REGS 13

/**
 * Sets the global exception handler function.
 *
 * Parameters:
 *   handler        The handler that will be called to handle exceptions.
 **/
void wramp_set_handler(void (*handler)(int estat));

/**
 * Loads the context of curr_scheduling_proc.
 *
 * Side Effects:
 *   This function does not return.
 **/
void wramp_load_context();

#endif
