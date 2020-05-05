#ifndef _DIRENT_H_
#define _DIRENT_H_

#include <sys/types.h>
#include <sys/dirent.h>

struct __dir{
    int fd;
    int pos;
    struct dirent buffer;
};

typedef struct __dir DIR;

int            closedir(DIR *);
DIR           *opendir(const char *);
struct dirent *readdir(DIR *);
int            readdir_r(DIR *, struct dirent *, struct dirent **);
void           rewinddir(DIR *);
void           seekdir(DIR *, long int);
long int       telldir(DIR *);

#endif

