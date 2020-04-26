#include "../fs.h"



int sys_read(struct proc *who,int fd, void *buf, size_t count){
    struct filp* file;
    SET_CALLER(who);
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    file = who->fp_filp[fd];
    return file->filp_dev->fops->read(file, buf, count, file->filp_pos);
}

int sys_write(struct proc *who,int fd, void *buf, size_t count){
    struct filp* file;
    SET_CALLER(who);
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    file = who->fp_filp[fd];
    return file->filp_dev->fops->write(file, buf, count, file->filp_pos);
}


