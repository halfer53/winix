#include <lib.h>

int open(const char *pathname,int flags, ...){
    struct message msg;
    msg.m1_p1 = (void*)pathname;
    msg.m1_i1 = flags;
    msg.m1_i2 = *((int *)&flags+1);
    return _syscall(OPEN, &msg);
}


int ioctl(int fd, unsigned long request, ...){
    struct message m;
    m.m1_i1 = fd;
    m.m1_i2 = request;
    m.m1_p1 = ((int *)&request) + 1;
    return _syscall(IOCTL, &m);
}

