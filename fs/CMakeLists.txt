cmake_minimum_required(VERSION 3.7)
project(fs)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        cache.c
        system/open_close.c
        dev.c
        filp.c
        fs.h
        fs_main.c
        inode.c
        makefs.c
        path.c
        system/read_write.c
        makefs_only/srec_import.c
        makefs_only/sys_stdio.c
        makefs_only/cmake_util.c make_disk.c makefs_only/cmake_util.h makefs_only/cmake_bitmap.c system/pipe.c makefs_only/tty.c system/stat.c system/lseek.c system/chdir_mkdir.c util.c system/chown_chmod.c system/dup.c system/link_unlink.c system/umask_access.c makefs_only/dir_util.c system/getdent.c makefs_only/srec_import.h unit_test.c system/sync.c system/mknod.c rootfs.c)

include_directories(../include/fs_include)
include_directories(../include)
add_compile_definitions(MAKEFS_STANDALONE)
add_compile_definitions(_DEBUG)

add_executable(fs ${SOURCE_FILES})