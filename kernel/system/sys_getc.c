#include "../winix.h"

/**
 * Syscall in this file: getc
 * Input:	
 *
 * Return: 	i1:	character read from serial port 1
 *
 * NB this is a blocking syscall
 * 
 */

int do_getc(struct proc *who, struct message *m){
    return kgetc();
}
