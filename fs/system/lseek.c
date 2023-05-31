//
// Created by bruce on 27/04/20.
//
#include <fs/fs.h>

int extend_file(struct filp* file, off_t count, int whence){
    off_t user_increment, allocated = 0;
    int ret;
    struct zone_iterator iter;
    struct inode* ino = file->filp_ino;
    int total_size = get_inode_total_size_word(ino);

    if (whence == SEEK_END)
        count += total_size;

    if( count < total_size){
        file->filp_pos = count;
        return count;
    }
    iter_zone_init(&iter, ino);
    user_increment = ALIGN1K( count - total_size);
    while(iter_zone_has_next(&iter));
    while(user_increment > 0){
        ret = iter_zone_alloc(&iter);
        if (ret < 0){
            iter_zone_close(&iter);
            return ret;
        }
        allocated += BLOCK_SIZE;
        user_increment -= BLOCK_SIZE;
    }
    iter_zone_close(&iter);
    return allocated;
}

int sys_lseek(struct proc* who, int fd, off_t offset, int whence){
    struct filp* file;
    int ret;
    if(!is_fd_opened_and_valid(who, fd))
        return -EBADF;
    file = who->fp_filp[fd];
    if(file->filp_ino->i_flags & INODE_FLAG_PIPE)
        return -ESPIPE;

    switch (whence) {
        case SEEK_SET:
            ret = extend_file(file, offset, whence);
            break;

        case SEEK_CUR:
            ret = extend_file(file, file->filp_pos + offset, whence);
            break;

        case SEEK_END:
            ret = extend_file(file, offset, whence);
            break;

        default:
            return -EINVAL;
    }
    return ret;
}


int do_lseek(struct proc* who, struct message* msg){
    return sys_lseek(who, msg->m1_i1, msg->m1_i2, msg->m1_i3);
}

