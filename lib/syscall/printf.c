#include <sys/syscall.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>


char estr[ESTR_SIZ];

char* strerror(int usrerrno){
    __strerror(estr, ESTR_SIZ, usrerrno);
    return estr;
}

void perror(const char *s){
    dprintf(STDERR_FILENO, "%s: %s\n", s, strerror(errno));
}
