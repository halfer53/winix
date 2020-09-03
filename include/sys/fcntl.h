#ifndef _SYS_FCNTL_H_
#define	_SYS_FCNTL_H_

/*
 * This file includes the definitions for open and fcntl
 * described by POSIX for <fcntl.h>.
 */

#include <sys/types.h>
#include <sys/stat.h>


/*
 * File status flags: these are used by open(2), fcntl(2).
 * They are also used (indirectly) in the kernel file structure f_flags,
 * which is a superset of the open/fcntl flags.  Open flags and f_flags
 * are inter-convertible using OFLAGS(fflags) and FFLAGS(oflags).
 * Open/fcntl flags begin with O_; kernel-internal flags begin with F.
 */
/* open-only flags */
#define	O_RDONLY	0x00000000	/* open for reading only */
#define	O_WRONLY	0x00000001	/* open for writing only */
#define	O_RDWR		0x00000002	/* open for reading and writing */
#define	O_ACCMODE	0x00000003	/* mask for above modes */

/* File status flags for open() and fcntl().  POSIX Table 6-5. */
#define O_APPEND       0x02000	/* set append mode */
#define O_NONBLOCK     0x04000	/* no delay */
#define O_REOPEN      0x010000	



/* Oflag values for open().  POSIX Table 6-4. */
#define O_CREAT        0x00100	/* creat file if it doesn't exist */
#define O_EXCL         0x00200	/* exclusive use flag */
#define O_NOCTTY       0x00400	/* do not assign a controlling terminal */
#define O_TRUNC        0x01000	/* truncate flag */

#define	O_DSYNC		0x00010000	/* write: I/O data completion */
#define	O_RSYNC		0x00020000	/* read: I/O completion as for write */
#define	O_ALT_IO	0x00040000	/* use alternate i/o semantics */
#define	O_DIRECT	0x00080000	/* direct I/O hint */
#define KO_NO_FD    0x00100000  /* do not return fd */

/*
 * Constants used for fcntl(2)
 */

/* command values */
/* These values are used for cmd in fcntl().  POSIX Table 6-1.  */
#define F_DUPFD            0	/* duplicate file descriptor */
#define F_GETFD	           1	/* get file descriptor flags */
#define F_SETFD            2	/* set file descriptor flags */
#define F_GETFL            3	/* get file status flags */
#define F_SETFL            4	/* set file status flags */
#define F_GETLK            5	/* get record locking information */
#define F_SETLK            6	/* set record locking information */
#define F_SETLKW           7	/* set record locking info; wait if blocked */
#define F_FREESP           8	/* free a section of a regular file */

/* File descriptor flags used for fcntl().  POSIX Table 6-2. */
#define FD_CLOEXEC         1	/* close on exec flag for third arg of fcntl */

/* record locking flags (F_GETLK, F_SETLK, F_SETLKW) */
#define F_RDLCK            1	/* shared or read lock */
#define F_WRLCK            2	/* exclusive or write lock */
#define F_UNLCK            3	/* unlock */

/*
 * Advisory file segment locking data type -
 * information passed to system by user
 */
// struct flock {
//   short l_type;			/* type: F_RDLCK, F_WRLCK, or F_UNLCK */
//   short l_whence;		/* flag for starting offset */
//   off_t l_start;		/* relative offset in bytes */
//   off_t l_len;			/* size; if 0, then until EOF */
//   pid_t l_pid;			/* process id of the locks' owner */
// };

/* Always ensure that these are consistent with <stdio.h> and <unistd.h>! */
#ifndef	SEEK_SET
#define	SEEK_SET	0	/* set file offset to offset */
#endif
#ifndef	SEEK_CUR
#define	SEEK_CUR	1	/* set file offset to current plus offset */
#endif
#ifndef	SEEK_END
#define	SEEK_END	2	/* set file offset to EOF plus offset */
#endif



// int	open(const char *, int, ...);
// int	creat(const char *, mode_t);
// int	fcntl(int, int, ...);


#endif /* !_SYS_FCNTL_H_ */
