//
// Created by bruce on 27/04/20.
//
#include "../fs.h"

int extend_file(struct filp* file, off_t count){
    off_t user_increment, allocated = 0;
    int pages, i;
    block_t bnr;
    struct inode* ino = file->filp_ino;

    if( count < file->filp_ino->i_total_size){
        file->filp_pos = count;
        return count;
    }
    user_increment = ALIGN1K( count - ino->i_total_size);
    for(i = 0; i < NR_TZONES; i++){
        bnr = ino->i_zone[i];
        if(bnr == 0 && user_increment > 0){
            bnr = alloc_block(ino, ino->i_dev);
            ino->i_zone[i] = bnr;
            allocated += BLOCK_SIZE;
            user_increment -= BLOCK_SIZE;
        }
    }
    return allocated;
}

int sys_lseek(struct proc* who, int fd, off_t offset, int whence){
    struct filp* file;
    int ret;
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    file = who->fp_filp[fd];
    if(file->filp_ino->flags & INODE_FLAG_PIPE)
        return ESPIPE;

    switch (whence) {
        case SEEK_SET:
            ret = extend_file(file, offset);
            break;

        case SEEK_CUR:
            ret = extend_file(file, file->filp_pos + offset);
            break;

        case SEEK_END:
            ret = extend_file(file, file->filp_ino->i_total_size + offset);
            break;

        default:
            return EINVAL;
    }
    return ret;
}


int do_lseek(struct proc* who, struct message* msg){
    return sys_lseek(who, msg->m1_i1, msg->m1_i2, msg->m1_i3);
}

