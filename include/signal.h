#ifndef _SIGNAL_H_
#define _SIGNAL_H_ 1


#include <stddef.h>
#include <sys/types.h>

/* Here are types that are closely associated with signal handling. */
typedef int sig_atomic_t;


#ifndef _SIGSET_T
#define _SIGSET_T 1
typedef unsigned int sigset_t;
#endif


#define _NSIG             21    /* number of signals used */

#define SIGHUP             1    /* hangup */
#define SIGINT             2    /* interrupt (DEL) */
#define SIGQUIT            3    /* quit (ASCII FS) */
#define SIGILL             4    /* illegal instruction */
#define SIGTRAP            5    /* trace trap (not reset when caught) */
#define SIGABRT            6    /* IOT instruction */
#define SIGIOT             6    /* SIGABRT for people who speak PDP-11 */
#define SIGUNUSED          7    /* spare code */
#define SIGFPE             8    /* floating point exception */
#define SIGKILL            9    /* kill (cannot be caught or ignored) */
#define SIGUSR1           10    /* user defined signal # 1 */
#define SIGSEGV           11    /* segmentation violation */
#define SIGUSR2           12    /* user defined signal # 2 */
#define SIGPIPE           13    /* write on a pipe with no one to read it */
#define SIGALRM           14    /* alarm clock */
#define SIGTERM           15    /* software termination signal from kill */
#ifndef _POSIX_SOURCE
#define    SIGURG         16    /* urgent condition on IO channel */
#endif
#define    SIGSTOP        17    /* sendable stop signal not from tty */
#define    SIGTSTP        18    /* stop signal from tty */
#define    SIGCONT        19    /* continue a stopped process */
#define    SIGCHLD        20    /* to parent on child stop or exit */


/* The sighandler_t type is not allowed unless _POSIX_SOURCE is defined. */
// typedef void _PROTOTYPE( (*sighandler_t), (int) );
typedef void (*sighandler_t)(int);

/* Macros used as function pointers. */
#define SIG_ERR     ((sighandler_t) 0xffff)    /* error return */
#define SIG_DFL       ((sighandler_t)  0)    /* default signal handling */
#define SIG_IGN       ((sighandler_t)  1)    /* ignore signal */

typedef struct stack{
  void  *ss_sp;     /* address of stack */
  int    sstate;  /* Flags */
  size_t ss_size;   /* Number of bytes in stack */
} stack_t;

struct sigaction {
  sighandler_t sa_handler;	/* SIG_DFL, SIG_IGN, or pointer to function */
  sigset_t sa_mask;		      /* signals to be blocked during handler */
  int sa_flags;			        /* special flags */
};

/* Fields for sa_flags. */
#define SA_ONSTACK   0x0001	/* deliver signal on alternate stack */
#define SA_RESETHAND 0x0002	/* reset signal handler when signal caught */
#define SA_NODEFER   0x0004	/* don't block signal while catching it */
#define SA_RESTART   0x0008	/* automatic system call restart */
#define SA_SIGINFO   0x0010	/* extended signal handling */
#define SA_NOCLDWAIT 0x0020	/* don't create zombies */
#define SA_NOCLDSTOP 0x0040	/* don't receive SIGCHLD when child stops */

/* POSIX requires these values for use with sigprocmask(2). */
#define SIG_BLOCK          0	/* for blocking signals */
#define SIG_UNBLOCK        1	/* for unblocking signals */
#define SIG_SETMASK        2	/* for setting the signal mask */

// define signal
void (*signal(int sig, void (*func)(int)))(int);

#define MINSIGSTKSZ 50

int sigaddset(sigset_t *, int);
int sigdelset(sigset_t *, int);
int sigemptyset(sigset_t *);
int sigfillset(sigset_t *);
int sigismember(sigset_t *, int);
int raise(int signum);
int kill (pid_t pid, int sig);
int sigaction(int signum, const struct sigaction *act,
                         struct sigaction *oldact);
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
int sigsuspend(const sigset_t *mask);
int sigpending(sigset_t *set);

#if defined(__wramp__) & !defined(_SYSTEM)

#define kill(pid, sig)                      wramp_syscall(KILL, pid, sig)
#define sigaction(signum, act, oact)        wramp_syscall(SIGACTION, signum, act, oact)
#define sigsuspend(mask)                    wramp_syscall(SIGSUSPEND, *mask)
#define sigpending(set)                     wramp_syscall(SIGPENDING, set)
#define signal(signum, handler)             wramp_syscall(SIGNAL, signum, handler)
#define sigprocmask(how, set, oldset)       wramp_syscall(SIGPROCMASK,how, oldset, *set)

#endif

#endif
