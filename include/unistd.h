#ifndef _UNISTD_H_
#define _UNISTD_H_ 1

#ifndef _POSIX_SOURCE
#define _POSIX_ 1
#endif

#include <sys/ioctl.h>
#include <stddef.h>
#include <sys/ipc.h>
#include <sys/unistd.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#include <errno.h>
#include <signal.h>

int isatty(int fd);
void sync();
int getdents( int fd, struct dirent *dirp, unsigned int count);
size_t read(int fd, void *buf, size_t count);
size_t write(int fd, const void *buf, size_t count);
int pipe(int pipefd[2]);
off_t lseek(int fd, off_t offset, int whence);

#if defined(__wramp__) & !defined(_SYSTEM)
#define sync()                              wramp_syscall(SYNC)
#define read(fd, buf, count)                wramp_syscall(READ,fd, buf, count)
#define write(fd, buf, count)               wramp_syscall(WRITE,fd, buf, count)
#define pipe(pipefd)                        wramp_syscall(PIPE, pipefd)
#define lseek(fd, offset, whence)           wramp_syscall(LSEEK, fd, offset, whence)
#endif

#endif
