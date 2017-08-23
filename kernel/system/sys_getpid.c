#include "../winix.h"

/**
 * Syscall in this file: getpid
 * Input:	
 *
 * Return: 	i1:	pid
 * 			i2: ppid
 * 
 */


int do_getpid(struct proc *who, struct message *m){
    m->i2 = who->parent;
    return who->proc_nr;
}
