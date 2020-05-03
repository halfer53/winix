
#ifndef _SYS_DIR_H_
#define _SYS_DIR_H_ 1

#include <sys/types.h>

#define    DIRBLKSIZ    1024    /* size of directory block */

// each direct occupies 32 words
#ifndef DIRSIZ
#define DIRSIZ    32
#endif

#ifndef DIRNAME_LEN
#define DIRNAME_LEN 29
#endif

struct dirent {
    ino_t          d_ino;       /* inode number */
    unsigned int   d_type;      /* type of file; not supported */
    char32_t    d_name[DIRNAME_LEN + 1]; /* filename */
};

#define	DT_UNKNOWN	 0
#define	DT_FIFO		 1
#define	DT_CHR		 2
#define	DT_DIR		 4
#define	DT_BLK		 6
#define	DT_REG		 8
#define	DT_LNK		10
#define	DT_SOCK		12

#endif /* _DIR_H */