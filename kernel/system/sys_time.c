/**
 * Syscall in this file: time
 * Input:	
 *
 * Return: 	i1: number of jittles since boot
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:10:29
 * 
*/
#include "../winix.h"

int do_time(struct proc *who, struct message *m){
    return system_uptime;
}
