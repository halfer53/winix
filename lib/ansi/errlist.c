#include <lib.h>


const char *_sys_errlist[_NERROR] = {
    0,			/* EGENERIC */
    "PERM",			/* EPERM */
    "NOENT",	/* ENOENT */
    "SRCH",		/* ESRCH */
    "INTR",	/* EINTR */
    "IO",			/* EIO */
    "NXIO",	/* ENXIO */
    "2BIG",		/* E2BIG */
    "NOEXEC",		/* ENOEXEC */
    "BADF",		/* EBADF */
    "CHILD",			/* ECHILD */
    "AGAIN",/* EAGAIN */
    "NOMEM",		/* ENOMEM */
    "ACCES",		/* EACCES */
    "FAULT",			/* EFAULT */
    "NOTBLK",	/* ENOTBLK */
    "BUSY",		/* EBUSY */
    "EXIST",			/* EEXIST */
    "XDEV",		/* EXDEV */
    "NODEV",		/* ENODEV */
    "NOTDIR",		/* ENOTDIR */
    "ISDIR",		/* EISDIR */
    "INVAL",		/* EINVAL */
    "NFILE",		/* ENFILE */
    "MFILE",		/* EMFILE */
    "NOTTY",		/* ENOTTY */
    "TXTBSY",		/* ETXTBSY */
    "FBIG",		/* EFBIG */
    "NOSPC",	/* ENOSPC */
    "SPIPE",			/* ESPIPE */
    "ROFS",	/* EROFS */
    "MLINK",		/* EMLINK */
    "PIPE",			/* EPIPE */
    "DOM",		/* EDOM */
    "RANGE",		/* ERANGE */

};
