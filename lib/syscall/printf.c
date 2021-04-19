#include <sys/syscall.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>


FILE _stdin =  {0, 0, _IODEFAULT, 0 , 0, 0};
FILE _stdout = {0, 1, _IODEFAULT, 0 , 0, 0};
FILE _stderr = {0, 2, _IODEFAULT, 0 , 0, 0};
FILE *stdin = &_stdin, *stdout = &_stdout, *stderr = &_stderr;

char estr[ESTR_SIZ];

// int __dprintf(int fd, const char *format, void* args){
//     struct message m;
//     m.m1_i1 = fd;
//     m.m1_p1 = (void *)format;
//     m.m1_p2 = args;
//     return _syscall(DPRINTF, &m);
// }

int dprintf(int fd, const char *format, ...){
    
    return __dprintf(fd, format, (int*)&format + 1);
}

int fprintf(FILE *stream, const char *format, ...){
    return __dprintf(stream->_fd, format, (int*)&format + 1);
}

int printf(const char *format, ...) {
    return __dprintf(1, format, (int*)&format + 1);
}

// int __strerror(char *buffer, int len){
//     struct message m;
//     m.m1_p1 = buffer;
//     m.m1_i1 = len;
//     m.m1_i2 = errno;
//     return _syscall(STRERROR, &m);
// }

char* strerror(int usrerrno){
    __strerror(estr, ESTR_SIZ, usrerrno);
    return estr;
}

// void perror(const char *s){
//     __strerror(estr, ESTR_SIZ, errno);
//     dprintf(2, "%s: %s\n", s, estr);
// }
