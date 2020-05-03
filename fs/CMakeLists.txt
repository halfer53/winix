cmake_minimum_required(VERSION 3.7)
project(fs)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        cache.c
        cache.h
        system/open_close.c
        dev.c
        filp.c
        fs.h
        fs_main.c
        inode.c
        makefs.c
        path.c
        path.h
        system/read_write.c
        cmake/cmake_util.c fs_test.c cmake/cmake_util.h disk.c cmake/cmake_bitmap.c system/pipe.c tty.c system/stat.c system/lseek.c system/chdir_mkdir.c miscellaneous.c system/chown_chmod.c system/dup.c system/link_unlink.c system/umask_access.c cmake/dir_util.c)

include_directories(../include)
add_compile_definitions(FS_CMAKE)
add_compile_definitions(_DEBUG)

add_executable(fs ${SOURCE_FILES})