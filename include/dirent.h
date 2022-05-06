#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <stddef.h>
#include <sys/dirent.h>

#if defined(__wramp__) & !defined(_SYSTEM)
#define getdents(fd, dirp, count)           (wramp_syscall(GETDENT, fd, dirp, count))
#endif

#endif

