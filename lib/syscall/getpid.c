#include <sys/syscall.h>
extern pid_t _pid;

pid_t getpid(){
    struct message m;
    _pid = _syscall(GETPID, &m);
    return _pid;
}

// ppid may change through init adoption, so it's not a
// good idea to cache it
pid_t getppid(){
    struct message m;
    _syscall(GETPID, &m);
    return m.m1_i2;
}
