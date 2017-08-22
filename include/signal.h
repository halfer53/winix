#ifndef _SIGNAL_H_
#define _SIGNAL_H_ 1


#include <sys/types.h>

/* Here are types that are closely associated with signal handling. */
typedef int sig_atomic_t;

#ifdef _POSIX_SOURCE
#ifndef _SIGSET_T
#define _SIGSET_T 1
typedef unsigned int sigset_t;
#endif
#endif

#define _NSIG             17	/* number of signals used */

#define SIGHUP             1	/* hangup */
#define SIGINT             2	/* interrupt (DEL) */
#define SIGQUIT            3	/* quit (ASCII FS) */
#define SIGILL             4	/* illegal instruction */
#define SIGTRAP            5	/* trace trap (not reset when caught) */
#define SIGABRT            6	/* IOT instruction */
#define SIGIOT             6	/* SIGABRT for people who speak PDP-11 */
#define SIGUNUSED          7	/* spare code */
#define SIGFPE             8	/* floating point exception */
#define SIGKILL            9	/* kill (cannot be caught or ignored) */
#define SIGUSR1           10	/* user defined signal # 1 */
#define SIGSEGV           11	/* segmentation violation */
#define SIGUSR2           12	/* user defined signal # 2 */
#define SIGPIPE           13	/* write on a pipe with no one to read it */
#define SIGALRM           14	/* alarm clock */
#define SIGTERM           15	/* software termination signal from kill */
#define SIGCHLD           17	/* child process terminated or stopped */


/* The sighandler_t type is not allowed unless _POSIX_SOURCE is defined. */
// typedef void _PROTOTYPE( (*sighandler_t), (int) );
typedef void (*sighandler_t)(int);

/* Macros used as function pointers. */
#define SIG_ERR    ((sighandler_t) -1)	/* error return */
#define SIG_DFL	   ((sighandler_t)  0)	/* default signal handling */
#define SIG_IGN	   ((sighandler_t)  1)	/* ignore signal */
#define SIG_HOLD   ((sighandler_t)  2)	/* block signal */
#define SIG_CATCH  ((sighandler_t)  3)	/* catch signal */


struct sigaction {
  // sighandler_t sa_handler;	/* SIG_DFL, SIG_IGN, or pointer to function */
  sighandler_t sa_handler;
  sigset_t sa_mask;		/* signals to be blocked during handler */
  int sa_flags;			/* special flags */
};

typedef struct stack{
  void  *ss_sp;     /* address of stack */
  int    ss_flags;  /* Flags */
  size_t ss_size;   /* Number of bytes in stack */
} stack_t;

//define signal
void (*signal(int sig, void (*func)(int)))(int);

//currently the minisal sigframe size is 35 words, we add an extra 10 for flexibility
#define MINSIGSTKSZ 45

#endif
