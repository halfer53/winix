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

#include <signal.h>

#ifndef _TYPES_H_
#include <sys/types.h>
#endif

#define _LOW(v)         ( (v) & 0x7f)
#define _HIGH(v)        ( ((v) >> 8) & 0xff)

#define WNOHANG         1    /* do not wait for child to exit */
#define WUNTRACED       2    /* for job control; not implemented */

#define WIFEXITED(s)        (_LOW(s) == 0)                /* normal exit */
#define WEXITSTATUS(s)      (_HIGH(s))                /* exit status */
#define WTERMSIG(s)         (_LOW(s))            /* sig value */
#define WIFSIGNALED(s)      (((unsigned int)(s)-1 & 0xFFFF) < 0xFF) /* signaled */
#define WIFSTOPPED(s)       (_LOW(s) == SIGSTOP || _LOW(s) == SIGTSTP )            /* stopped */
#define WSTOPSIG(s)         (_HIGH(s))                    /* stop signal */

// pid_t wait(int *wstatus);
// pid_t waitpid(pid_t pid, int *wstatus, int options);

#endif /* _WAIT_H */

