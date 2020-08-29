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

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <signal.h>


#define _NSYSCALL               48
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

#define DECLARE_SYSCALL(function, params, syscall_num, passing_codes)\
function params{\
    struct message m;\
    passing_codes;\
    return _syscall(syscall_num, &m);\
}\


#define WINFO_PS    1
#define WINFO_MEM   2
#define WINFO_SLAB  4
#define WINFO_TRACE_SYSCALL (8)

#ifndef MAKEFS_STANDALONE

int _syscall(int syscall_num, struct message *m);
int direct_wramp_syscall(int num, ...);

int ___exit(int status);
int sys_ps();
pid_t fork();
pid_t vfork();
int brk(void *addr);
void *sbrk(int incr);
void perror(const char *s);
char* strerror(int usrerrno);
unsigned int alarm(unsigned int seconds);
pid_t wait(int *wstatus);
pid_t waitpid(pid_t pid, int *wstatus, int options);
pid_t getpid();
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
int enable_syscall_tracing();

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

#endif

#endif

#endif
