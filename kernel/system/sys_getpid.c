/**
 * Syscall in this file: getpid
 * Input:	
 *
 * Return: 	i1:	pid
 * 			i2: ppid
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * @create date 2017-08-23 06:09:42
 * @modify date 2017-08-23 06:09:42
*/
#include "../winix.h"


int do_getpid(struct proc *who, struct message *m){
    m->i2 = who->parent;
    return who->proc_nr;
}
