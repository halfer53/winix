#include <fs/common.h>
#include <fs/cache.h>
#include <kernel/proc.h>
#include <fs/super.h>
#include "../makefs_only/cmake_util.h"
#include "../makefs_only/srec_import.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <argp.h>
#include <stdlib.h>
#include <winix/welf.h>
#include <dirent.h>
#include <fs/fs_methods.h>
#include <winix/list.h>
#include <winix/dev.h>
#include <winix/compiler.h>
#include <fs/super.h>

void wramp_unit_test1(){
    
}

int main(int argc, char **argv){
    init_os(_DISK_RAW, DISK_SIZE, false);
    printf("wramp disk laoded successfully\n");
}