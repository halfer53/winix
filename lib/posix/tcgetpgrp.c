#include <unistd.h>
#include <sys/ioctl.h>

pid_t tcgetpgrp(int fd){
    pid_t pid;
    ioctl(fd, TIOCGPGRP, &pid);
    return pid;
}