#include <sys/syscall.h>
#include <stddef.h>
#include <stdio.h>

FILE _stdin =  {0, STDIN_FILENO, _IODEFAULT, 0 , 0, 0};
FILE _stdout = {0, STDOUT_FILENO, _IODEFAULT, 0 , 0, 0};
FILE _stderr = {0, STDERR_FILENO, _IODEFAULT, 0 , 0, 0};

int dprintf(int fd, const char *format, ...){
    
    return __dprintf(fd, format, (int*)&format + 1);
}
