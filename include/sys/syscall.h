/**
 * 
 * Userspace system call library for WINIX.
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/


#ifndef _SYSCALL_H_
#define _SYSCALL_H_ 1

#include <sys/types.h>
#include <sys/dirent.h>
#include <sys/statfs.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>


#ifndef NULL
#define	NULL		((void *)0)
#endif

#define _NSYSCALL               55
/**
 * System Call Numbers
 **/
#define NONE            0   // not used
#define TIMES           1
#define EXIT            2
#define FORK            3
#define VFORK           4
#define EXECVE          5
#define BRK             6
#define ALARM           7
#define SIGACTION       8
#define SIGRET          9
#define WAITPID         10
#define KILL            11
#define GETPID          12
#define WINFO           13
#define STRERROR        14
#define DPRINTF         15
#define SYSCONF         16
#define SIGSUSPEND      17
#define SIGPENDING      18
#define SIGPROCMASK     19
#define SETPGID         20
#define GETPGID         21
#define OPEN            22
#define READ            23
#define WRITE           24
#define CLOSE           25
#define CREAT           26
#define PIPE            27
#define MKNOD           28
#define CHDIR           29
#define CHOWN           30
#define CHMOD           31
#define STAT            32
#define FSTAT           33
#define DUP             34
#define DUP2            35
#define LINK            36
#define UNLINK          37
#define GETDENT         38
#define ACCESS          39
#define MKDIR           40
#define SYNC            41
#define LSEEK           42
#define UMASK           43
#define FCNTL           44
#define IOCTL           45
#define SETSID          46
#define CSLEEP          47
#define GETPPID         48
#define SIGNAL          49
#define SBRK            50
#define STATFS          51
#define GETCWD          52
#define TFORK           53
#define SCHED_YIELD     54


#define WINFO_PS            1
#define WINFO_MEM           2
#define WINFO_SLAB          3
#define WINFO_TRACE_SYSCALL (4)
#define WINFO_DISABLE_TRACE (5)
#define WINFO_DEBUG_IPC     6
#define WINFO_DEBUG_SCHEDULING  7

extern const char **_environ;

#ifndef MAKEFS_STANDALONE

// int _syscall(int syscall_num, struct message *m);
int wramp_syscall(int num, ...);
void *ptr_wramp_syscall(int num, ...);

void exit(int status);
pid_t getppid();
int kill (pid_t pid, int sig);
long sysconf(int name);
int sigaction(int signum, const struct sigaction *act,
                         struct sigaction *oldact);
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
int sigsuspend(const sigset_t *mask);
int sigpending(sigset_t *set);
int setpgid(pid_t pid, pid_t pgid);
pid_t getpgid(pid_t pid);
int open(const char *pathname,int flags, ...);
// int open(const char *pathname,int flags, mode_t mode);
int creat(const char *pathname, mode_t mode);
int close(int fd);
size_t read(int fd, void *buf, size_t count);
size_t write(int fd, const void *buf, size_t count);
int pipe(int pipefd[2]);
int access(const char *pathname, int mode);
int chdir(const char *path);
int mkdir(const char *pathname, mode_t mode);
int getdents( int fd, struct dirent *dirp, unsigned int count);
int execve(const char *pathname, char *const argv[],char *const envp[]);
int execv(const char *path, char *const argv[]);
off_t lseek(int fd, off_t offset, int whence);
mode_t umask(mode_t mask);
int sync();
int unlink(const char *pathname);
int link(const char *oldpath, const char *newpath);
int dup2(int oldfd, int newfd);
int dup(int oldfd);
int fstat(int fd, struct stat *statbuf);
int stat(const char *pathname, struct stat *statbuf);
int chmod(const char *pathname, mode_t mode);
int chown(const char *pathname, uid_t owner, gid_t group);
int mknod(const char *pathname, mode_t mode, dev_t dev);
int fcntl(int fd, int cmd, ... /* arg */ );
pid_t setsid(void);
int ioctl(int fd, unsigned long request, ...);
int csleep(int ticks);
int dprintf(int fd, const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int printf(const char *format, ...);
int enable_syscall_tracing();
int disable_syscall_tracing();
void* get_sigreturn_func_ptr(void);
void *sbrk(int increment);
char *strerror(int err);
int statfs(const char *path, struct statfs *buf);
char *getcwd(char *buf, size_t size);
clock_t times(struct tms *buf);
pid_t waitpid(pid_t pid, int *status, int options);
pid_t tfork();
int brk(void *addr);
void init_environ();
void sched_yield();

#ifdef __wramp__
#ifndef _SYSTEM

#define sync()                      (wramp_syscall(SYNC))
#define getdents(fd, dirp, count)   (wramp_syscall(GETDENT, fd, dirp, count))
#define creat(pathname, mode)       (wramp_syscall(CREAT, mode, pathname))
#define close(fd)                   (wramp_syscall(CLOSE, fd))
#define read(fd, buf, count)        (wramp_syscall(READ,fd, buf, count))
#define write(fd, buf, count)       (wramp_syscall(WRITE,fd, buf, count))
#define pipe(pipefd)                (wramp_syscall(PIPE, pipefd))
#define mknod(pathname, mode, dev)  (wramp_syscall(MKNOD, mode, pathname, dev))
#define chdir(path)                 (wramp_syscall(CHDIR, path))
#define chown(pathname, owner, group)(wramp_syscall(CHOWN, owner, pathname, group))
#define chmod(pathname, mode)       (wramp_syscall(CHMOD, mode, pathname))
#define stat(pathname, statbuf)     (wramp_syscall(STAT, pathname, statbuf))
#define fstat(fd, statbuf)          (wramp_syscall(FSTAT, fd, statbuf))
#define dup(oldfd)                  (wramp_syscall(DUP, oldfd))
#define dup2(oldfd, newfd)          (wramp_syscall(DUP2, oldfd, newfd))
#define link(oldpath, newpath)      (wramp_syscall(LINK, oldpath, newpath))
#define unlink(pathname)            (wramp_syscall(UNLINK, pathname))
#define access(pathname, mode)      (wramp_syscall(ACCESS, mode, pathname))
#define mkdir(pathname, mode)       (wramp_syscall(MKDIR, mode, pathname))
#define lseek(fd, offset, whence)   (wramp_syscall(LSEEK, fd, offset, whence))
#define umask(mask)                 (wramp_syscall(UMASK, mask))
#define alarm(seconds)              (wramp_syscall(ALARM, seconds))
#define csleep(ticks)               (wramp_syscall(CSLEEP, ticks))
#define execve(path, argv, envp)    (wramp_syscall(EXECVE, path, argv, envp))
#define fork()                      (wramp_syscall(FORK))
#define getpgid(pid)                (wramp_syscall(GETPGID, pid))
#define getpid()                    (wramp_syscall(GETPID))
#define kill(pid, sig)              (wramp_syscall(KILL, pid, sig))
#define __dprintf(fd, format, arg)  (wramp_syscall(DPRINTF, fd, format, arg))
#define __strerror(buffer, len,usrerr)     (wramp_syscall(STRERROR, len, buffer, usrerr))
#define setpgid(pid, pgid)          (wramp_syscall(SETPGID, pid, pgid))
#define setsid()                    (wramp_syscall(SETSID))
#define sigaction(signum, act, oact)(wramp_syscall(SIGACTION, act, oact, get_sigreturn_func_ptr(), signum))
#define sigsuspend(mask)            (wramp_syscall(SIGSUSPEND, *mask))
#define sysconf(name)               (wramp_syscall(SYSCONF, name))
#define sigprocmask(how, set, oldset)   (wramp_syscall(SIGPROCMASK,how, oldset, *set))
#define sigpending(set)             (wramp_syscall(SIGPENDING, set))
#define times(buf)                  (wramp_syscall(TIMES, buf))
#define waitpid(pid, wstatus, option)   (wramp_syscall(WAITPID, pid, wstatus, option))
#define wait(wstatus)               (waitpid(-1, wstatus, 0))
#define enable_syscall_tracing()    (wramp_syscall(WINFO, WINFO_TRACE_SYSCALL))
#define disable_syscall_tracing()    (wramp_syscall(WINFO, WINFO_DISABLE_TRACE))
#define vfork()                     (wramp_syscall(VFORK))
#define exit(status)                (wramp_syscall(EXIT, status, 0))
#define getppid()                   (wramp_syscall(GETPPID))
#define signal(signum, handler)     (wramp_syscall(SIGNAL, signum, get_sigreturn_func_ptr(), handler))
#define sbrk(increment)             (ptr_wramp_syscall(SBRK, increment))
#define brk(ptr)                    (wramp_syscall(BRK, ptr))
#define statfs(path, buf)           (wramp_syscall(STATFS, path, buf))
#define getcwd(buf, size)           (ptr_wramp_syscall(GETCWD, size, buf))
#define tfork()                     (wramp_syscall(TFORK))
#define sched_yield()               (wramp_syscall(SCHED_YIELD))

#endif //_SYSTEM

#define __ALIGN1K(x) 	    (((((x))>>10)<<10)+1023)
#define __get_env_address() (__ALIGN1K((int)get_sp()))
#define __get_env_ptr()     (*((const char ***) __get_env_address()))
#define __get_env()         (_environ ? _environ : __get_env_ptr()) 
#define init_environ()      (_environ = __get_env())

#define execv(path, argv)       execve(path, argv, __get_env()) 
#define perror(s)               dprintf(STDERR_FILENO, "%s: %s\n", s, strerror(errno))

#endif //__wramp__

#endif //MAKEFS_STANDALONE

#endif
