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

#define _NSYSCALL               22
/**
 * System Call Numbers
 **/
#define SYSCALL_NONE            0   //not used
#define SYSCALL_TIMES           1
#define SYSCALL_EXIT            2
#define SYSCALL_FORK            3
#define SYSCALL_VFORK           4
#define SYSCALL_EXECVE          5
#define SYSCALL_BRK             6
#define SYSCALL_ALARM           7
#define SYSCALL_SIGACTION          8
#define SYSCALL_SIGRET          9
#define SYSCALL_WAITPID            10
#define SYSCALL_KILL            11
#define SYSCALL_GETPID          12
#define SYSCALL_WINFO           13
#define SYSCALL_GETC            14
#define SYSCALL_PRINTF          15
#define SYSCALL_SYSCONF         16
#define SYSCALL_SIGSUSPEND      17
#define SYSCALL_SIGPENDING      18
#define SYSCALL_SIGPROCMASK     19
#define SYSCALL_SETPGID         20
#define SYSCALL_GETPGID         21

#define DECLARE_SYSCALL(function, params, syscall_num, passing_codes)\
function params{\
    struct message m;\
    int __ret;\
    m.type = syscall_num;\
    passing_codes;\
    __ret = winix_sendrec(SYSTEM,&m);\
    if(m.reply_res < 0){\
        __errno = -m.reply_res;\
        return -1;\
    }\
    return m.reply_res;\
}\


#define WINFO_PS    1
#define WINFO_MEM   2
#define WINFO_SLAB  4


int _syscall(int syscall_num, struct message *m);
int _exit(int status);
int sys_ps();
pid_t fork();
pid_t vfork();
int brk(void *addr);
void *sbrk(int incr);
int getc();
unsigned int alarm(unsigned int seconds);
pid_t wait(int *wstatus);
pid_t waitpid(pid_t pid, int *wstatus, int options);
pid_t getpid();
pid_t getppid();
int kill (pid_t pid, int sig);
long sysconf(int name);
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
int sigsuspend(const sigset_t *mask);
int sigpending(sigset_t *set);

#endif
