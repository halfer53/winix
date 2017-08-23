#include "../winix.h"

/**
 * Syscall in this file: time
 * Input:	
 *
 * Return: 	i1: number of jittles since boot
 * 
 */

int do_time(struct proc *who, struct message *m){
    return system_uptime;
}
