#include "fs.h"
#include <winix/list.h>

void init_fs() {
    init_buf();
    init_inode();
    init_filp();
    init_root_fs();
    init_pipe();
}

