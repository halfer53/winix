#include <sys/syscall.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>

FILE _stdin =  {0, 0, _IODEFAULT, 0 , 0, 0};
FILE _stdout = {0, 1, _IODEFAULT, 0 , 0, 0};
FILE _stderr = {0, 2, _IODEFAULT, 0 , 0, 0};
FILE *stdin = &_stdin, *stdout = &_stdout, *stderr = &_stderr;

char estr[ESTR_SIZ];

int dprintf(int fd, const char *format, ...){
    
    return __dprintf(fd, format, (int*)&format + 1);
}

int fprintf(FILE *stream, const char *format, ...){
    return __dprintf(stream->_fd, format, (int*)&format + 1);
}

int printf(const char *format, ...) {
    return __dprintf(1, format, (int*)&format + 1);
}

char* strerror(int usrerrno){
    __strerror(estr, ESTR_SIZ, usrerrno);
    return estr;
}

void perror(const char *s){
    dprintf(STDERR_FILENO, "%s: %s\n", s, strerror(errno));
}
