/**
 * 
 * Syscall context struct definition
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:39
 * 
*/
#ifndef _W_SYSCALL_CTX_H_
#define _W_SYSCALL_CTX_H_ 1

#include <ucontext.h>

typedef int (*syscall_handler_t)(struct proc* who, struct message* m);

struct syscall_ctx{
    struct message m;
    struct proc *who;
    ucontext_t ctx;
    int interruptted;
};

#endif

