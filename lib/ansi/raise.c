#include <lib.h>

int raise(int signum){
    return kill(getpid(),signum);
}
