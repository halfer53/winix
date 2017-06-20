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

extern proc_t *current_proc;
extern struct super_block *sb;

int hexstr2int(char *a, int len);
void int2hexstr(char *buffer,int n, int bytenr);

#endif

