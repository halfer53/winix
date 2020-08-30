#include <sys/syscall.h>
#include <signal.h>

void* get_sigreturn_func_ptr(void);

sighandler_t signal(int signum, sighandler_t handler){
    struct sigaction sa, oldsa;
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESETHAND;
    sa.sa_mask = 0xffff;
    if(sigaction(signum, &sa, &oldsa))
        return SIG_ERR;
    return oldsa.sa_handler;
}

