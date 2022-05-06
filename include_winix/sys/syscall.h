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

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dirent.h>
#include <sys/statfs.h>
#include <sys/times.h>
#include <sys/debug.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>

#define __ALIGN1K(x) 	    (((((x))>>10)<<10)+1023)
#define __get_env_address() (__ALIGN1K((unsigned long)get_sp()))
#define __get_env_ptr()     (*((const char ***) (unsigned long)__get_env_address()))
#define __get_env()         (_environ ? _environ : __get_env_ptr()) 
#define init_environ()      (_environ = __get_env())

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
#define EXECVE          (5)
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

int wramp_syscall(int num, ...);
void *ptr_wramp_syscall(int num, ...);

#define CHECK_PRINTF    __attribute__ ((format (printf, 1, 2)))
#define CHECK_EPRINTF    __attribute__ ((format (printf, 2, 3)))

#ifndef FSUTIL

// int _syscall(int syscall_num, struct message *m);







int dprintf(int fd, const char *format, ...) CHECK_EPRINTF;
int fprintf(FILE *stream, const char *format, ...) CHECK_EPRINTF;
int printf(const char *format, ...) CHECK_PRINTF;
int enable_syscall_tracing();
int disable_syscall_tracing();
void* get_sigreturn_func_ptr(void);

char *strerror(int err);



void perror();


#if defined(__wramp__) & !defined(_SYSTEM)



#define fprintf(stream, format, ...)        dprintf(stream->_fd, format, ##__VA_ARGS__)
#define printf(format, ...)                 dprintf(STDOUT_FILENO, format, ##__VA_ARGS__)

#define __dprintf(fd, format, arg)          wramp_syscall(DPRINTF, fd, format, arg)
#define __strerror(buffer, len,usrerr)      wramp_syscall(STRERROR, len, buffer, usrerr)
#define __exit(status)                      wramp_syscall(EXIT, status, 0)



#define enable_syscall_tracing()            wramp_syscall(WINFO, WINFO_TRACE_SYSCALL)
#define disable_syscall_tracing()           wramp_syscall(WINFO, WINFO_DISABLE_TRACE)




#endif //__wramp__

#endif //FSUTIL

#endif
