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
int unlink(const char *pathname);
int link(const char *oldpath, const char *newpath);
int dup2(int oldfd, int newfd);
int dup(int oldfd);
unsigned int alarm(unsigned int seconds);
int csleep(int ticks);
pid_t fork();
pid_t vfork();
pid_t tfork();
pid_t getpgid(pid_t pid);
pid_t getpid();
int setpgid(pid_t pid, pid_t pgid);
pid_t setsid(void);
long sysconf(int name);
void _exit(int status);
pid_t getppid();
void *sbrk(int increment);
int brk(void *addr);
char *getcwd(char *buf, size_t size);
int execve(const char *pathname, char *const argv[],char *const envp[]);
int execv(const char *path, char *const argv[]);

#if defined(__wramp__) & !defined(_SYSTEM)

#define sync()                              wramp_syscall(SYNC)
#define read(fd, buf, count)                wramp_syscall(READ,fd, buf, count)
#define write(fd, buf, count)               wramp_syscall(WRITE,fd, buf, count)
#define pipe(pipefd)                        wramp_syscall(PIPE, pipefd)
#define lseek(fd, offset, whence)           wramp_syscall(LSEEK, fd, offset, whence)
#define dup(oldfd)                          wramp_syscall(DUP, oldfd)
#define dup2(oldfd, newfd)                  wramp_syscall(DUP2, oldfd, newfd)
#define link(oldpath, newpath)              wramp_syscall(LINK, oldpath, newpath)
#define unlink(pathname)                    wramp_syscall(UNLINK, pathname)
#define alarm(seconds)                      wramp_syscall(ALARM, seconds)
#define csleep(ticks)                       wramp_syscall(CSLEEP, ticks)
#define fork()                              wramp_syscall(FORK)
#define vfork()                             wramp_syscall(VFORK)
#define tfork()                             wramp_syscall(TFORK)
#define getpgid(pid)                        wramp_syscall(GETPGID, pid)
#define getpid()                            wramp_syscall(GETPID)
#define setpgid(pid, pgid)                  wramp_syscall(SETPGID, pid, pgid)
#define setsid()                            wramp_syscall(SETSID)
#define sysconf(name)                       wramp_syscall(SYSCONF, name)
#define getppid()                           wramp_syscall(GETPPID)
#define sbrk(increment)                     ptr_wramp_syscall(SBRK, increment)
#define brk(ptr)                            wramp_syscall(BRK, ptr)
#define getcwd(buf, size)                   ptr_wramp_syscall(GETCWD, size, buf)
#define execve(path, argv, envp)            wramp_syscall(EXECVE, path, argv, envp)
#define execv(path, argv)                   execve(path, argv, __get_env())

#define _exit(status)                       __exit(status)

#endif

#endif
