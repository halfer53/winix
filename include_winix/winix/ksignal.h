/**
 * 
 * kernel signal 
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:29
 * 
*/
#ifndef _W_SIGNAL_H_
#define _W_SIGNAL_H_ 1

#include <winix/syscall_ctx.h>
#include <signal.h>

// By POSIX convention, if a process is killed by a signal, its exit status code is 128 + signum
#define SIG_STATUS(s)   (128 + (s))


int send_sig(struct proc *who, int signum);
int cause_sig(struct proc *who, int signum);
int is_sigpending(struct proc* who);
int handle_sig(struct proc* who, int signum);
int handle_pendingsig(struct proc* who);

struct sigframe{
    int signum;
    int syscall_num;
    uintptr_t code;
    uintptr_t code2; 
};

#endif
