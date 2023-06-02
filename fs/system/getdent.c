//
// Created by bruce on 3/05/20.
//

#include <fs/fs.h>

int sys_getdents(struct proc* who, int fd, struct dirent* dirp_dst, unsigned int count){
    struct dirent_iterator iter;
    struct filp* file;
    struct inode* dirp;
    struct winix_dirent* dirstream;
    int ret = 0;

    if(!is_fd_opened_and_valid(who, fd))
        return -EBADF;

    file = who->fp_filp[fd];
    dirp = file->filp_ino;

    if(!S_ISDIR(dirp->i_mode))
        return -ENOTDIR;

    _iter_dirent_init(&iter, dirp, file->getdents_zone_nr, file->getdents_dirstream_nr, false);
    while(iter_dirent_has_next(&iter)){
        if(!count)
            break;
        
        dirstream = iter_dirent_get_next(&iter);
        if(dirstream->dirent.d_ino > 0) {
            memcpy(dirp_dst++, &dirstream->dirent, sizeof(struct dirent));
            ret += sizeof(struct dirent);
            count--;
        }
    }
    file->getdents_zone_nr = iter.zone_iter.i_zone_idx - 1;
    file->getdents_dirstream_nr = iter.dirent - (struct winix_dirent*)iter.buffer->block;
    iter_dirent_close(&iter);
    return ret;
}

int do_getdents(struct proc* who, struct message* msg){
    struct dirent* path = (struct dirent *) get_physical_addr(msg->m1_p1, who);
    int count = msg->m1_i2;
    if(!is_vaddr_ok(msg->m1_p1, sizeof(struct dirent) * count, who))
        return -EFAULT;
    return sys_getdents(who, msg->m1_i1, path, count);
}

