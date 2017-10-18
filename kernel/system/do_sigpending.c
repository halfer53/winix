#include <kernel/kernel.h>
/**
 * Syscall in this file: sigpending
 * Input:   
 *
 * Return:  m1_i1: pending masks
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:10:17
 * 
*/
int do_sigpending(struct proc* who, struct message* m){
    m->m1_i1 = who->sig_pending;
    return OK;
}

