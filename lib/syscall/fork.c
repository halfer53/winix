#include <sys/syscall.h>

pid_t _pid = 0;//pid cache

pid_t fork(){
    int result;
    struct message m;
    result = _syscall(SYSCALL_FORK,&m);
    if(result == 0)
        _pid = 0; //reset pid cache if its child
    return result;
}
