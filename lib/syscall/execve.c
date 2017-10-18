#include <sys/syscall.h>


int execve(){
    struct message m;
    return _syscall(SYSCALL_EXECVE,&m);
}
