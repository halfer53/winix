#include <sys/syscall.h>


int execve(){
    struct message m;
    return _syscall(EXECVE,&m);
}
