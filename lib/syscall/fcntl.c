#include <sys/syscall.h>

int fcntl(int fd, int cmd, ... /* arg */ ){
    struct message m;
    m.m1_i1 = fd;
    m.m1_i2 = cmd;
    m.m1_p1 = (int*)&cmd + 1;
    return _syscall(FCNTL, &m);
}