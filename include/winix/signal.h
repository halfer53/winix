#ifndef _W_SIGNAL_H_
#define _W_SIGNAL_H_

#include <signal.h>

void send_signal(proc_t *who, int signum);
void set_signal(proc_t *caller, int signum, sighandler_t handler);
void do_sigreturn(int signum);


#endif