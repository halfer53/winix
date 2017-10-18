#include <sys/syscall.h>
#include <unistd.h>


int setpgid(pid_t pid, pid_t pgid){
    struct message m;
    m.m1_i1 = pid;
    m.m1_i2 = pgid;
    return _syscall(SYSCALL_SETPGID, &m);
}
