#ifndef _W_SIGNAL_H_
#define _W_SIGNAL_H_ 1

#include <signal.h>

#define SIGFRAME_CODE_LEN   2
//By POSIX convention, if a process is killed by a signal, its exit status code is 128 + signum
#define KILL_PROC(proc,sigcode) exit_proc(proc, 128+(sigcode));


int send_sig(proc_t *who, int signum);
int cause_sig(proc_t *who, int signum);
int set_signal(proc_t *who, int signum, sighandler_t handler);

typedef struct{
    int signum;

    int operation;
    int dest;
    message_t *pm;

}sigframe_t;

#endif
