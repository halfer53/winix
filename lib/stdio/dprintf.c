#include <sys/syscall.h>
#include <stddef.h>
#include <stdio.h>

FILE _stdin =  {0, 0, _IODEFAULT, 0 , 0, 0};
FILE _stdout = {0, 1, _IODEFAULT, 0 , 0, 0};
FILE _stderr = {0, 2, _IODEFAULT, 0 , 0, 0};

int dprintf(int fd, const char *format, ...){
    
    return __dprintf(fd, format, (int*)&format + 1);
}
