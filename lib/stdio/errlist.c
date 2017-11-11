#include <lib.h>


const char *_errlist[_NERROR] = {
    0,			/* EGENERIC */
    "Not owner",			/* EPERM */
    "No such file or directory",	/* ENOENT */
    "No such process",		/* ESRCH */
    "Interrupted system call",	/* EINTR */
    "I/O error",			/* EIO */
    "No such device or address",	/* ENXIO */
    "Arg list too long",		/* E2BIG */
    "Exec format error",		/* ENOEXEC */
    "Bad file number",		/* EBADF */
    "No children",			/* ECHILD */
    "Resource temporarily unavailable",/* EAGAIN */
    "Not enough core",		/* ENOMEM */
    "Permission denied",		/* EACCES */
    "Bad address",			/* EFAULT */
    "Block device required",	/* ENOTBLK */
    "Resource busy",		/* EBUSY */
    "File exists",			/* EEXIST */
    "Cross-device link",		/* EXDEV */
    "No such device",		/* ENODEV */
    "Not a directory",		/* ENOTDIR */
    "Is a directory",		/* EISDIR */
    "Invalid argument",		/* EINVAL */
    "File table overflow",		/* ENFILE */
    "Too many open files",		/* EMFILE */
    "Not a typewriter",		/* ENOTTY */
    "Text file busy",		/* ETXTBSY */
    "File too large",		/* EFBIG */
    "No space left on device",	/* ENOSPC */
    "Illegal seek",			/* ESPIPE */
    "Read-only file system",	/* EROFS */
    "Too many links",		/* EMLINK */
    "Broken pipe",			/* EPIPE */
    "Math argument",		/* EDOM */
    "Result too large",		/* ERANGE */
    "Resource deadlock avoided",	/* EDEADLK */
    "File name too long",		/* ENAMETOOLONG */
    "No locks available",		/* ENOLCK */
    "Function not implemented",	/* ENOSYS */
    "Directory not empty",		/* ENOTEMPTY */

};
