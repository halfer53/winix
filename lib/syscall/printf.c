#include <sys/syscall.h>

int __dprintf(int fd, const char *format, void* args){
    struct message m;
    m.m1_i1 = fd;
    m.m1_p1 = (void *)format;
    m.m1_p2 = args;
    return _syscall(DPRINTF, &m);
}

int dprintf(int fd, const char *format, ...){
    
    return __dprintf(fd, format, (int*)&format + 1);
}

int printf(const char *format, ...) {
    return __dprintf(1, format, (int*)&format + 1);
}
