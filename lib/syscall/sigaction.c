#include <sys/syscall.h>
#include <signal.h>

void __sigreturn(void);

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact){
    struct message m;
    m.m1_i1 = signum;
    m.m1_p1 = (void *)act;
    m.m1_p2 = (void *)oldact;
    m.m1_p3 = (void *)(__sigreturn);
    return _syscall(SYSCALL_SIGNAL, &m);
}

sighandler_t signal(int signum, sighandler_t handler){
    struct sigaction sa, oldsa;
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESETHAND;
    sigfillset(&sa.sa_mask);
    if(sigaction(signum, &sa, &oldsa))
        return SIG_ERR;
    return oldsa.sa_handler;
}
