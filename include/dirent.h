#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <stddef.h>
#include <sys/dirent.h>

int getdents( int fd, struct dirent *dirp, unsigned int count);

#if defined(__wramp__) & !defined(LINTING) && !defined(_SYSTEM)
#define getdents(fd, dirp, count)           wramp_syscall(GETDENT, fd, dirp, count)
#endif

#endif

