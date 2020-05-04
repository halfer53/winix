//
// Created by bruce on 3/05/20.
//

#include <fs/fs.h>

int sys_getdent(struct proc* who, int fd, struct dirent* dirp_dst){
    int i, dirstream_nr;
    block_buffer_t *buffer;
    struct dirent* dirstream, *endstream;
    struct filp* file;
    struct inode* dirp;
    block_t bnr;

    if(!is_fd_opened_and_valid(who, fd))
        return EACCES;

    file = who->fp_filp[fd];
    dirp = file->filp_ino;

    if(!S_ISDIR(dirp->i_mode))
        return ENOTDIR;

    dirstream_nr = file->getdent_dirstream_nr;
    i = file->getdent_zone_nr;
    for(; i < NR_TZONES; i++){
        bnr = dirp->i_zone[i];
        if(bnr > 0){
            buffer = get_block_buffer(bnr, dirp->i_dev);
            endstream = (struct dirent*)&buffer->block[BLOCK_SIZE];
            dirstream = (struct dirent*)buffer->block;
            dirstream += dirstream_nr;
            while(dirstream->d_ino == 0 && dirstream < endstream){
                dirstream++;
                dirstream_nr++;
            }
            if(dirstream >= endstream){
                file->getdent_zone_nr++;
                put_block_buffer(buffer);
                continue;
            }
            *dirp_dst = *dirstream;
            dirstream_nr++;
            file->getdent_dirstream_nr = dirstream_nr;
            return put_block_buffer(buffer);
        }
    }
    return ENOENT;
}

int do_getdent(struct proc* who, struct message* msg){
    struct dirent* path = (struct dirent *) get_physical_addr(msg->m1_p1, who);
    return sys_getdent(who, msg->m1_i1, path);
}