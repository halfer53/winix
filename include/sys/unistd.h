#ifndef _SYS_UNISTD_H_
#define _SYS_UNISTD_H_ 1

#include <sys/dirent.h>

/* access function */
#define    F_OK        0    /* test for existence of file */
#define    X_OK        0x01    /* test for execute or search permission */
#define    W_OK        0x02    /* test for write permission */
#define    R_OK        0x04    /* test for read permission */


/* configurable pathname variables */
#define    _PC_LINK_MAX        1
#define    _PC_MAX_CANON        2
#define    _PC_MAX_INPUT        3
#define    _PC_NAME_MAX        4
#define    _PC_PATH_MAX        5
#define    _PC_PIPE_BUF        6
#define    _PC_CHOWN_RESTRICTED    7
#define    _PC_NO_TRUNC        8
#define    _PC_VDISABLE        9

/* configurable system variables */
#define    _SC_ARG_MAX        1
#define    _SC_CHILD_MAX        2
#define    _SC_CLK_TCK        3
#define    _SC_NGROUPS_MAX        4
#define    _SC_OPEN_MAX        5
#define    _SC_JOB_CONTROL        6
#define    _SC_SAVED_IDS        7
#define    _SC_VERSION        8


DIR *opendir(const char *pathname);
struct dirent *readdir(DIR *directory);
int closedir(DIR *directory);
const char **get_environ();

#define environ     (__get_env())


#endif /* !_SYS_UNISTD_H_ */
