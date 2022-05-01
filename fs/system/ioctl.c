#include <fs/fs.h>

int _sys_ioctl(struct proc* who, int fd, int request, void* arg){
    struct filp* file;
    if(!is_fd_opened_and_valid(who, fd))
        return -EBADF;
    if(!is_vaddr_accessible(arg, who))
        return -EFAULT;
    file = who->fp_filp[fd];
    return file->filp_dev->fops->ioctl(file, request, arg);
}

int do_ioctl(struct proc* who, struct message* msg){
    return _sys_ioctl(who, msg->m1_i1, msg->m1_i2, msg->m1_p1);
}


