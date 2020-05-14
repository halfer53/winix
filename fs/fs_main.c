#include "fs.h"
#include <winix/list.h>

void init_fs() {

    init_buf();
    init_inode();
    init_filp();
    init_dev();
    init_root_fs();
    init_pipe();
#ifdef FILE_SYSTEM_PROJECT
    init_tty();
#endif
}

