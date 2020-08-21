#include "../fs.h"

int _sys_ioctl(struct proc* who, int fd, int request, void* arg){
    struct filp* file;
    ptr_t* ptr;
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    if(!is_vaddr_accessible(arg, who))
        return EFAULT;
    file = who->fp_filp[fd];
    ptr = get_physical_addr(arg, who);
    return file->filp_dev->fops->ioctl(file, request, ptr);
}

int do_ioctl(struct proc* who, struct message* msg){
    return _sys_ioctl(who, msg->m1_i1, msg->m1_i2, msg->m1_p1);
}


