#ifndef _FS_FS_H_
#define _FS_FS_H_

#define NULL ((void*)0)

#include <stdio.h>
#include "string.h"
#include "const.h"
#include "inode.h"
#include "cache.h"
#include "file.h"
#include "dev.h"
#include "path.h"
#include "filp.h"
#include "makefs.h"
#include "proc.h"
#include "super.h"

#include "open.h"
#include "read.h"
#include "close.h"



#ifndef EOF
# define EOF (-1)
#endif
#ifndef ERR
#define ERR (-1)	/* general error flag */
#endif
#ifndef OK
#define OK 0		/* general OK flag */
#endif

#ifndef _DIR_H_
#define _DIR_H_

#define	DIRBLKSIZ	1024	/* size of directory block */

//each direct occupies 32 bytes, with 8 bytes for d_ino, and 24 bytes for directory name
#ifndef DIRSIZ
#define DIRSIZ	32
#endif

#ifndef DIRNAME_LEN
#define DIRNAME_LEN 24
#endif

struct direct {
  ino_t d_ino;
  char d_name[DIRNAME_LEN];
};

#endif /* _DIR_H */

#define S_IFDIR 0x004000  	/* directory */

/* open-only flags */
#define	O_RDONLY	0x0000		/* open for reading only */
#define	O_WRONLY	0x0001		/* open for writing only */
#define	O_RDWR		0x0002		/* open for reading and writing */
#define	O_ACCMODE	0x0003		/* mask for above modes */
#define	O_CREAT		0x0200		/* create if nonexistant */


extern proc_t *current_proc;
extern struct super_block *sb;

int hexstr2int(char *a, int len);
char hexstr2char(char A);
void int2hexstr(char *buffer,int n, int bytenr);

#endif

