#include "fs.h"
#include <winix/list.h>

void init_fs(disk_word_t* disk_raw, size_t disk_size_words) {

    init_buf();
    init_inode();
    init_filp();
    init_dev();
    init_root_fs();
    init_pipe();

}

