/**
 * 
 * WINIX System Task.
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


#ifndef _K_SYSTEM_H_
#define _K_SYSTEM_H_ 1

/**
 * Entry point for the system task.
 **/
void system_main();
struct message *curr_mesg();
void intr_syscall();

#endif
