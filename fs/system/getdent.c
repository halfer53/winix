//
// Created by bruce on 3/05/20.
//

#include "../fs.h"

int sys_getdent(struct proc* who, int fd, struct dirent* dirp_dst){
    int i, dirstream_nr;
    block_buffer_t *buffer;
    struct dirent* dirstream, *endstream;
    struct filp* file;
    struct inode* dirp;

    if(!is_fd_opened_and_valid(who, fd))
        return EACCES;

    file = who->fp_filp[fd];
    dirp = file->filp_ino;

    if(!S_ISDIR(dirp->i_mode))
        return ENOTDIR;

    dirstream_nr = who->getdent_dirstream_nr;
    i = who->getdent_zone_nr;
    for(; i < NR_TZONES; i++){
        if(dirp->i_zone[i] > 0 && (buffer = get_block_buffer(dirp->i_zone[i], dirp->i_dev)) != NULL){
            endstream = (struct dirent*)&buffer->block[BLOCK_SIZE];
            dirstream = (struct dirent*)buffer->block;
            dirstream += dirstream_nr;
            while(dirstream->d_ino == 0 && dirstream < endstream){
                dirstream++;
                dirstream_nr++;
            }
            if(dirstream >= endstream){
                who->getdent_zone_nr++;
                put_block_buffer(buffer);
                continue;
            }
            *dirp_dst = *dirstream;
            dirstream_nr++;
            who->getdent_dirstream_nr = dirstream_nr;
            return put_block_buffer(buffer);
        }
    }
    return ENOENT;
}