//
// Created by bruce on 10/05/20.
//
#include "../fs.h"

int sys_sync(struct proc* who){
    return flush_all_buffer();
}

