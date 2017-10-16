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

#include <signal.h>

//By POSIX convention, if a process is killed by a signal, its exit status code is 128 + signum
#define SIG_STATUS(s)   (128 + (s))


int send_sig(struct proc *who, int signum);
int set_signal(struct proc *who, int signum, sighandler_t handler);

#define SIG_FRAME_CODE_LEN  (2)

struct sigframe_code{
    unsigned int codes[SIG_FRAME_CODE_LEN];
};

struct sigframe{
    struct syscall_frame_comm s_base;
    struct sigframe_code s_codes;
};



#endif
