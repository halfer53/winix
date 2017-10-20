#include <sys/syscall.h>


int printf(const char *format, ...) {
    struct message m;
    m.m1_p1 = (void *)format;
    m.m1_p2 = (void *)((int *)&format+1);
    return _syscall(PRINTF,&m);
}
