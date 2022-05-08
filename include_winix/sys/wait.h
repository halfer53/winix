/* The <sys/wait.h> header contains macros related to wait(). The value
 * returned by wait() and waitpid() depends on whether the process 
 * terminated by an exit() call, was killed by a signal, or was stopped
 * due to job control, as follows:
 *
 *                 High byte   Low byte
 *                +---------------------+
 *    exit(status)        |  status  |    0     |
 *                +---------------------+
 *      killed by signal    |    0     |  signal  |
 *                +---------------------+
 *    stopped (job control)    |  signal  |   0177   |
 *                +---------------------+
 */

#ifndef _WAIT_H_
#define _WAIT_H_ 1

#include <sys/types.h>

#define _LOW(v)         ( (v) & 0x7f)
#define _HIGH(v)        ( ((v) >> 8) & 0xff)

#define WNOHANG         1    /* do not wait for child to exit */
#define WUNTRACED       2    /* return if a child has stopped */

#define WIFEXITED(s)        (_LOW(s) == 0)                                          /* normal exit */
#define WEXITSTATUS(s)      (_HIGH(s))                                              /* exit status */
#define WTERMSIG(s)         (_LOW(s))                                               /* sig value */
#define WIFSIGNALED(s)      (_LOW(s))                                               /* signaled */
#define WIFSTOPPED(s)       (_LOW(s) == SIGSTOP || _LOW(s) == SIGTSTP )             /* stopped */
#define WSTOPSIG(s)         (_LOW(s))                                               /* stop signal */

pid_t wait(int *wstatus);
pid_t waitpid(pid_t pid, int *wstatus, int options);

#if defined(__wramp__) & !defined(LINTING) && !defined(_SYSTEM)

#define waitpid(pid, wstatus, option)       wramp_syscall(WAITPID, pid, wstatus, option)
#define wait(wstatus)                       waitpid(-1, wstatus, 0)

#endif

#endif /* _WAIT_H */

