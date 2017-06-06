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