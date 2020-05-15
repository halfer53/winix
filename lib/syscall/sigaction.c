#include <sys/syscall.h>
#include <signal.h>

void __sigreturn(void);

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact){
    struct message m;
    m.m3_i1 = signum;
    m.m3_p1 = (char *)act;
    m.m3_p2 = (char *)oldact;
    m.m3_f1 = __sigreturn;
    return _syscall(SIGACTION, &m);
}

sighandler_t signal(int signum, sighandler_t handler){
    struct sigaction sa, oldsa;
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESETHAND;
    sa.sa_mask = 0xffff;
    if(sigaction(signum, &sa, &oldsa))
        return SIG_ERR;
    return oldsa.sa_handler;
}

