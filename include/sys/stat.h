#ifndef _SYS_STAT_H_
#define	_SYS_STAT_H_

#include <sys/types.h>		/* XXX */


struct stat {
    dev_t     st_dev;         /* ID of device containing file */
    ino_t     st_ino;         /* Inode number */
    mode_t    st_mode;        /* File type and mode */
    nlink_t   st_nlink;       /* Number of hard links */
    uid_t     st_uid;         /* User ID of owner */
    gid_t     st_gid;         /* Group ID of owner */
    dev_t     st_rdev;        /* Device ID (if special file) */
    off_t     st_size;        /* Total size, in bytes */
    blksize_t st_blksize;     /* Block size for filesystem I/O */
    blkcnt_t  st_blocks;      /* Number of 512B blocks allocated */
    time_t st_atime;		/* time of last access */
    time_t st_mtime;		/* time of last data modification */
    time_t st_ctime;		/* time of last file status change */
};



#define	S_ISUID	0x0004000			/* set user id on execution */
#define	S_ISGID	0x0002000			/* set group id on execution */

#define	S_IRWXU	0x0000700			/* RWX mask for owner */
#define	S_IRUSR	0x0000400			/* R for owner */
#define	S_IWUSR	0x0000200			/* W for owner */
#define	S_IXUSR	0x0000100			/* X for owner */

#define	S_IRWXG	0x0000070			/* RWX mask for group */
#define	S_IRGRP	0x0000040			/* R for group */
#define	S_IWGRP	0x0000020			/* W for group */
#define	S_IXGRP	0x0000010			/* X for group */

#define	S_IRWXO	0x0000007			/* RWX mask for other */
#define	S_IROTH	0x0000004			/* R for other */
#define	S_IWOTH	0x0000002			/* W for other */
#define	S_IXOTH	0x0000001			/* X for other */

#define	S_IFMT	 0x0170000		/* type of file mask */
#define	S_IFIFO  0x0010000		/* named pipe (fifo) */
#define	S_IFCHR  0x0020000		/* character special */
#define	S_IFDIR  0x0040000		/* directory */
#define	S_IFBLK  0x0060000		/* block special */
#define	S_IFREG  0x0100000		/* regular */
#define	S_IFLNK  0x0120000		/* symbolic link */
#define	S_IFSOCK 0x0140000		/* socket */
#define	S_ISVTX  0x0001000		/* save swapped text even after use */


#define	S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)	/* directory */
#define	S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)	/* char special */
#define	S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)	/* block special */
#define	S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)	/* regular file */
#define	S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)	/* fifo */
#define	S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)	/* symbolic link */
#define	S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)	/* socket */

// int	chmod(const char *, mode_t);
// int	mkdir(const char *, mode_t);
// int	mkfifo(const char *, mode_t);
// int	stat(const char *, struct stat *);
// int	fstat(int, struct stat *);
// mode_t	umask(mode_t);
// int	fchmod(int, mode_t);

// int	lstat(const char *, struct stat *);
// int	mknod(const char *, mode_t, dev_t);

#endif /* !_SYS_STAT_H_ */
