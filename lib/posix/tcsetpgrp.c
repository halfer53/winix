#include <unistd.h>
#include <sys/ioctl.h>

int tcsetpgrp(int fd, pid_t pgrp){
    return ioctl(fd, TIOCSPGRP, &pgrp);
}