#include <lib.h>

int raise(int signum){
    if(signum < 0 || signum >= _NSIG)
        return -1;
    return kill(getpid(),signum);
}

int abort(){
    return kill(getpid(),SIGABRT);
}
