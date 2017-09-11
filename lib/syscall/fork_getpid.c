#include <lib.h>


static pid_t _pid = 0;//pid cache

pid_t fork(){
    int result;
    struct message m;
    result = _syscall(SYSCALL_FORK,&m);
    if(result == 0)
        _pid = 0; //reset pid cache if its child
    return result;
}

pid_t getpid(){
    struct message m;
    if(_pid != 0){
        return _pid;
    }
    _pid = _syscall(SYSCALL_GETPID,&m);
    return _pid;
}

//ppid may change through init adoption, so it's not a
//good idea to cache it
pid_t getppid(){
    struct message m;
    _syscall(SYSCALL_GETPID,&m);
    return m.m1_i2;
}
