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


#define _NSYSCALL               44
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
#define WINIX_STRERROR          14
#define WINIX_DPRINTF          15
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
int ___exit(int status);
int sys_ps();
pid_t fork();
pid_t vfork();
int brk(void *addr);
void *sbrk(int incr);
void perror(const char *s);
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
int getdent( int fd, struct dirent *dirp);
int execve(const char *pathname, char *const argv[],char *const envp[]);
int execv(const char *path, char *const argv[]);
off_t lseek(int fd, off_t offset, int whence);
mode_t umask(mode_t mask);
int sync();
int getdent( int fd, struct dirent *dirp);
int unlink(const char *pathname);
int link(const char *oldpath, const char *newpath);
int dup2(int oldfd, int newfd);
int dup(int oldfd);
int fstat(int fd, struct stat *statbuf);
int stat(const char *pathname, struct stat *statbuf);
int chmod(const char *pathname, mode_t mode);
int chown(const char *pathname, uid_t owner, gid_t group);
int mknod(const char *pathname, mode_t mode, dev_t dev);
int printf(const char *format, ...);


#endif

#endif
