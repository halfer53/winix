#include <sys/syscall.h>
#include <signal.h>

void* get_sigreturn_func_ptr(void);

// int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact){
//     struct message m;
//     m.m1_i1 = signum;
//     m.m1_p1 = (char *)act;
//     m.m1_p2 = (char *)oldact;
//     m.m1_p3 = get_sigreturn_func_ptr();
//     return _syscall(SIGACTION, &m);
// }

sighandler_t signal(int signum, sighandler_t handler){
    struct sigaction sa, oldsa;
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESETHAND;
    sa.sa_mask = 0xffff;
    if(sigaction(signum, &sa, &oldsa))
        return SIG_ERR;
    return oldsa.sa_handler;
}

