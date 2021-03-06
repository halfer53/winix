/**
 * Syscall in this file: getc
 * Input:    
 *
 * Return:     reply_res:    character read from serial port 1
 *
 * NB this is a blocking syscall
 * 
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:09:33
 * 
*/
#include <kernel/kernel.h>

int do_getc(struct proc *who, struct message *m){
    return kgetc_blocking(who);
}
