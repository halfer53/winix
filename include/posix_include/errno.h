/* The <errno.h> header defines the numbers of the various errors that can
 * occur during program execution.  They are visible to user programs and 
 * should be small positive integers.  However, they are also used within 
 * MINIX, where they must be negative.  For example, the READ system call is 
 * executed internally by calling do_read().  This function returns either a 
 * (negative) error number or a (positive) number of bytes actually read.
 *
 * To solve the problem of having the error numbers be negative inside the
 * the system and positive outside, the following mechanism is used.  All the
 * definitions are are the form:
 *
 *    #define EPERM        (_SIGN 1)
 *
 * If the macro _SYSTEM is defined, then  _SIGN is set to "-", otherwise it is
 * set to "".  Thus when compiling the operating system, the  macro _SYSTEM
 * will be defined, setting EPERM to (- 1), whereas when when this
 * file is included in an ordinary user program, EPERM has the value ( 1).
 */

#ifndef _ERRNO_H_        /* check if <errno.h> is already included */
#define _ERRNO_H_ 1        /* it is not included; note that fact */

#include <sys/debug.h>

/* Now define _SIGN as "" or "-" depending on _SYSTEM. */
#ifdef _SYSTEM
#   define _SIGN         -
#define errno  
#define __set_errno(val)   
#else
#   define _SIGN 
#define ERRNO_PTR           ((int *)( (((unsigned int)get_sp()) >> 10 << 10) + 1))
#define errno   (*ERRNO_PTR)
#define __set_errno(val)    ((*ERRNO_PTR) = (val))        
#endif

#define ESTR_SIZ    (16)
extern char estr[ESTR_SIZ];

/* Here are the numerical values of the error numbers. */
#define _NERROR              40  /* number of errors */  

#define EPERM         (_SIGN  1)  /* operation not permitted */
#define ENOENT        (_SIGN  2)  /* no such file or directory */
#define ESRCH         (_SIGN  3)  /* no such process */
#define EINTR         (_SIGN  4)  /* interrupted function call */
#define EIO           (_SIGN  5)  /* input/output error */
#define ENXIO         (_SIGN  6)  /* no such device or address */
#define E2BIG         (_SIGN  7)  /* arg list too long */
#define ENOEXEC       (_SIGN  8)  /* exec format error */
#define EBADF         (_SIGN  9)  /* bad file descriptor */
#define ECHILD        (_SIGN 10)  /* no child process */
#define EAGAIN        (_SIGN 11)  /* resource temporarily unavailable */
#define ENOMEM        (_SIGN 12)  /* not enough space */
#define EACCES        (_SIGN 13)  /* permission denied */
#define EFAULT        (_SIGN 14)  /* bad address */
#define ENOTBLK       (_SIGN 15)  /* Extension: not a block special file */
#define EBUSY         (_SIGN 16)  /* resource busy */
#define EEXIST        (_SIGN 17)  /* file exists */
#define EXDEV         (_SIGN 18)  /* improper link */
#define ENODEV        (_SIGN 19)  /* no such device */
#define ENOTDIR       (_SIGN 20)  /* not a directory */
#define EISDIR        (_SIGN 21)  /* is a directory */
#define EINVAL        (_SIGN 22)  /* invalid argument */
#define ENFILE        (_SIGN 23)  /* too many open files in system */
#define EMFILE        (_SIGN 24)  /* too many open files */
#define ENOTTY        (_SIGN 25)  /* inappropriate I/O control operation */
#define ETXTBSY       (_SIGN 26)  /* no longer used */
#define EFBIG         (_SIGN 27)  /* file too large */
#define ENOSPC        (_SIGN 28)  /* no space left on device */
#define ESPIPE        (_SIGN 29)  /* invalid seek */
#define EROFS         (_SIGN 30)  /* read-only file system */
#define EMLINK        (_SIGN 31)  /* too many links */
#define EPIPE         (_SIGN 32)  /* broken pipe */
#define EDOM          (_SIGN 33)  /* domain error        (from ANSI C std) */
#define ERANGE        (_SIGN 34)  /* result too large    (from ANSI C std) */
#define EDEADLK       (_SIGN 35)  /* resource deadlock avoided */
#define ENAMETOOLONG  (_SIGN 36)  /* file name too long */
#define ENOLCK        (_SIGN 37)  /* no locks available */
#define ENOSYS        (_SIGN 38)  /* function not implemented */
#define ENOTEMPTY     (_SIGN 39)  /* directory not empty */



#endif /* _ERRNO_H */

