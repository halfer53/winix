#include <sys/syscall.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

void perror(const char *s){
    dprintf(STDERR_FILENO, "%s: %s\n", s, strerror(errno));
}
