#ifndef _FS_FS_H_
#define _FS_FS_H_

#define NULL ((void*)0)

#include "const.h"
#include "inode.h"
#include "lru.h"
#include "file.h"


#ifndef EOF
# define EOF (-1)
#endif

#ifndef _DIR_H_
#define _DIR_H_

#include <sys/types.h>

#define	DIRBLKSIZ	1024	/* size of directory block */

#ifndef DIRSIZ
#define DIRSIZ	32
#endif

struct direct {
  ino_t d_ino;
  char d_name[DIRSIZ];
};

#endif /* _DIR_H */


#endif

