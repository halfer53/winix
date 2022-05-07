#include <unistd.h>
#include <signal.h>

int raise(int signum){
    return kill(getpid(),signum);
}
