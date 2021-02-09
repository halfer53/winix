//
// Created by bruce on 3/05/20.
//

#include "../fs.h"

int sys_getdents(struct proc* who, int fd, struct dirent* dirp_dst, unsigned int count){
    int i, dirstream_nr, ret = 0;
    struct block_buffer *buffer;
    struct winix_dirent* dirstream, *endstream;
    struct filp* file;
    struct inode* dirp;
    block_t bnr;

    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;

    file = who->fp_filp[fd];
    dirp = file->filp_ino;

    if(!S_ISDIR(dirp->i_mode))
        return ENOTDIR;

    dirstream_nr = file->getdents_dirstream_nr;
    i = file->getdents_zone_nr;
    for(; i < NR_TZONES; i++){
        bnr = dirp->i_zone[i];
        if(bnr > 0){
            // KDEBUG(("getting block %d for inode %d\n", bnr, dirp->i_num));
            buffer = get_block_buffer(bnr, dirp->i_dev);
            endstream = (struct winix_dirent*)&buffer->block[BLOCK_SIZE];
            dirstream = (struct winix_dirent*)buffer->block;
            dirstream += dirstream_nr;

            while(dirstream < endstream){
                if(!count)
                    break;

                if(dirstream->dirent.d_ino > 0) {
                    memcpy(dirp_dst++, &dirstream->dirent, sizeof(struct dirent));
                    ret += sizeof(struct dirent);
                    file->getdents_dirstream_nr = dirstream_nr + 1;
                    count--;
                }
                dirstream++;
                dirstream_nr++;
            }
            if(count){
                file->getdents_zone_nr++;
                dirstream_nr = 0;
            }
            put_block_buffer(buffer);
        }
    }
    return ret;
}

int do_getdents(struct proc* who, struct message* msg){
    struct dirent* path = (struct dirent *) get_physical_addr(msg->m1_p1, who);
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return EFAULT;
    return sys_getdents(who, msg->m1_i1, path, msg->m1_i2);
}

