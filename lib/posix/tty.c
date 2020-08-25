#include <sys/syscall.h>
#include <sys/macro.h>
#include <sys/stat.h>
#include <sys/tty.h>

int isatty(int fd){
    struct stat statbuf;

    int ret = fstat(fd, &statbuf);
    if (!ret) 
    {
        return DEV_MAJOR(statbuf.st_dev) == TTY_MAJOR_NUM;
    }
    return 0;
}



