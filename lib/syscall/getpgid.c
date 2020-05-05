#include <sys/syscall.h>
#include <unistd.h>


pid_t getpgid(pid_t pid){
    struct message m;
    m.m1_i1 = pid;
    _syscall(GETPGID, &m);
    return m.m1_i1;
}
