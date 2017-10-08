cmake_minimum_required(VERSION 3.7)
project(fs)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        cache.c
        cache.h
        close.c
        const.h
        dev.c
        dev.h
        file.h
        filp.c
        filp.h
        fs.h
        fs_main.c
        inode.c
        inode.h
        makefs.c
        makefs.h
        open.c
        path.c
        path.h
        proc.h
        read.c
        super.h
        util.c string.c string.h read.h open.h close.h)

add_executable(fs ${SOURCE_FILES})