#ifndef _FCNTL_H_
#define _FCNTL_H_

#include <sys/fcntl.h>
#include <sys/syscall.h>

int creat(const char *pathname, mode_t mode);
int close(int fd);
int open(const char *pathname,int flags, ...);
int fcntl(int fd, int cmd, ... /* arg */ );

#if defined(__wramp__) & !defined(LINTING) && !defined(_SYSTEM)

#define creat(pathname, mode)               wramp_syscall(CREAT, mode, pathname)
#define close(fd)                           wramp_syscall(CLOSE, fd)
#define open(path, flags, ...)              wramp_syscall(OPEN, flags, path, ##__VA_ARGS__)
#define fcntl(fd, cmd, ...)                 wramp_syscall(FCNTL, fd, cmd, ##__VA_ARGS__)

#endif

#endif
