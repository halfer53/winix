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


#define WINFO_PS                1
#define WINFO_MEM               2
#define WINFO_SLAB              3
#define WINFO_TRACE_SYSCALL     4
#define WINFO_DISABLE_TRACE     5
#define WINFO_DEBUG_IPC         6
#define WINFO_DEBUG_SCHEDULING  7
#define WINFO_NO_GPF            8

int wramp_syscall(int num, ...);
void *ptr_wramp_syscall(int num, ...);

int enable_syscall_tracing();
int disable_syscall_tracing();


#if defined(__wramp__)

#define __dprintf(fd, format, arg)          wramp_syscall(DPRINTF, fd, format, arg)
#define __strerror(buffer, len,usrerr)      wramp_syscall(STRERROR, len, buffer, usrerr)
#define __exit(status)                      wramp_syscall(EXIT, status, 0)
#define enable_syscall_tracing()            wramp_syscall(WINFO, WINFO_TRACE_SYSCALL)
#define disable_syscall_tracing()           wramp_syscall(WINFO, WINFO_DISABLE_TRACE)

#endif


#endif
