//
// Created by bruce on 10/05/20.
//
#include "../fs.h"

int do_sync(struct proc* who, struct message* msg){
    flush_all_buffer();
    flush_super_block(get_dev(ROOT_DEV));
    return OK;
}



