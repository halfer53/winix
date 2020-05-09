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
        excluded_files/srec_import.c
        excluded_files/cmake_util.c make_disk.c excluded_files/cmake_util.h excluded_files/cmake_bitmap.c system/pipe.c tty.c system/stat.c system/lseek.c system/chdir_mkdir.c util.c system/chown_chmod.c system/dup.c system/link_unlink.c system/umask_access.c excluded_files/dir_util.c system/getdent.c excluded_files/srec_import.h fs_test.c)

include_directories(../include/fs_include)
include_directories(../include)
add_compile_definitions(FILE_SYSTEM_PROJECT)
add_compile_definitions(_DEBUG)

add_executable(fs ${SOURCE_FILES})