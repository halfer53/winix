
#ifndef	_SYS_DIRENT_H_
#define _SYS_DIRENT_H_ 1

#include <uchar.h>
#include <sys/stat.h>
#include <sys/limits.h>

#define    DIRBLKSIZ    1024    /* size of directory block */

struct dirent {
    ino_t          d_ino;       /* inode number */
    unsigned int   d_type;      /* type of file; not supported */
    char32_t   d_name[WINIX_NAME_LEN ]; /* filename */
};

struct winix_dirent {
    struct dirent dirent;
    dev_t dev;
};

#define DIR_BUFFER_LEN  (10)

struct __dir{
    int fd;
    int pos;
    int limit;
    int is_cached;
    struct dirent buffer[DIR_BUFFER_LEN];
};

typedef struct __dir DIR;

#define	DT_UNKNOWN	 0
#define	DT_FIFO		 1
#define	DT_CHR		 2
#define	DT_DIR		 4
#define	DT_BLK		 6
#define	DT_REG		 8
#define	DT_LNK		10
#define	DT_SOCK		12

int            closedir(DIR *);
DIR           *opendir(const char *);
struct dirent *readdir(DIR *);
int            readdir_r(DIR *, struct dirent *, struct dirent **);
void           rewinddir(DIR *);
void           seekdir(DIR *, long int);
long int       telldir(DIR *);

#endif /* _DIR_H */

