/**
 * 
 * Winix kernel stdio
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
#include <kernel/kernel.h>
#include <kernel/clock.h>
#include <winix/rex.h>
#include <ctype.h>
#include <winix/dev.h>

const char *errlist[_NERROR] = {
    0,			/* EGENERIC */    
    "EPERM",    /* EPERM */
    "ENOENT",    /* ENOENT */
    "ESRCH",    /* ESRCH */
    "EINTR",    /* EINTR */
    "EIO",			/* EIO */    
    "ENXIO",    /* ENXIO */
    "E2BIG",    /* E2BIG */
    "ENOEXEC",    /* ENOEXEC */
    "EBADF",    /* EBADF */
    "ECHILD",    /* ECHILD */
    "EAGAIN",    /* EAGAIN */
    "ENOMEM",    /* ENOMEM */
    "EACCES",    /* EACCES */
    "EFAULT",    /* EFAULT */
    "ENOTBLK",    /* ENOTBLK */
    "EBUSY",    /* EBUSY */
    "EEXIST",    /* EEXIST */
    "EXDEV",		/* EXDEV */    
    "ENODEV",    /* ENODEV */
    "ENOTDIR",    /* ENOTDIR */
    "EISDIR",    /* EISDIR */
    "EINVAL",    /* EINVAL */
    "ENFILE",    /* ENFILE */
    "EMFILE",    /* EMFILE */
    "ENOTTY",    /* ENOTTY */
    "ETXTBSY",    /* ETXTBSY */
    "EFBIG",    /* EFBIG */
    "ENOSPC",    /* ENOSPC */
    "ESPIPE",    /* ESPIPE */
    "EROFS",	/* EROFS */    
    "EMLINK",    /* EMLINK */
    "EPIPE",    /* EPIPE */
    "EDOM",    /* EDOM */
    "ERANGE",    /* ERANGE */
    "EDEADLK",    /* EDEADLK */
    "ENAMETOOLONG",    /* ENAMETOOLONG */
    "ENOLCK",    /* ENOLCK */
    "ENOSYS",    /* ENOSYS */
    "ENOTEMPTY",    /* ENOTEMPTY */
};

const char* kstr_error(int err){
    if(err < 0){
        err = -err;
    }
    if(err < 0 || err >= _NERROR)
        return (const char*)0;
    return errlist[err];
}

int tty_write_rex(RexSp_t* rex, char* data, size_t len){
    return 0;
}

char *strlcpy(char *dest, const char *src, size_t n){
    char *saved = dest;
    while (*src && n--) {
        *dest++ = *src++;
    }
    *dest = 0;
    return saved;
}



