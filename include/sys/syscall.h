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

#define _NSYSCALL               22
/**
 * System Call Numbers
 **/
#define NONE            0   //not used
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
#define GETC            14
#define PRINTF          15
#define SYSCONF         16
#define SIGSUSPEND      17
#define SIGPENDING      18
#define SIGPROCMASK     19
#define SETPGID         20
#define GETPGID         21

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
int sigaction(int signum, const struct sigaction *act,
                         struct sigaction *oldact);
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
int sigsuspend(const sigset_t *mask);
int sigpending(sigset_t *set);

#endif
