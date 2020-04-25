cmake_minimum_required(VERSION 3.7)
project(fs)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        cache.c
        cache.h
        open_close.c
        dev.c
        filp.c
        fs.h
        fs_main.c
        inode.c
        makefs.c
        path.c
        path.h
        read_write.c
        util.c string.c string.h fs_test.c util_cmake.h disk.c bitmap.c pipe.c tty.c)

include_directories(../include)
add_compile_definitions(FS_CMAKE)
add_compile_definitions(_DEBUG)

add_executable(fs ${SOURCE_FILES})