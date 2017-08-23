/**
 * 
 * WINIX System Task.
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * @create date 2016-09-19
 * @modify date 2017-08-23 06:04:55
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
