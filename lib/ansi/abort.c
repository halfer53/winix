#include <unistd.h>
#include <signal.h>

void abort(){
    kill(getpid(), SIGABRT);
}

