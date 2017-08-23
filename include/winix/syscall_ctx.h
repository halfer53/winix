/**
 * 
 * Syscall context struct definition
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * @create date 2017-08-23 06:22:39
 * @modify date 2017-08-23 06:22:39
*/
#ifndef _W_SYSCALL_CTX_H_
#define _W_SYSCALL_CTX_H_ 1

#include <ucontext.h>

struct syscall_ctx{
    struct message m;
    struct proc *who;
    ucontext_t ctx;
    int interruptted;
};

#endif

