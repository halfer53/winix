#include "../fs.h"

int _sys_ioctl(struct proc* who, int fd, int request, void* arg){
    struct filp* file;
    ptr_t* ptr;
    struct device* dev;
    struct tty_private* tty_data;
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    if(!is_vaddr_accessible(arg, who))
        return EFAULT;
    file = who->fp_filp[fd];
    dev = file->filp_dev;
    tty_data = (struct tty_private*)dev->private;
    ptr = get_physical_addr(arg, who);
    switch (request)
    {
    case TIOCGPGRP:
        break;
    case TIOCSPGRP:
        break;

    default:
        return EINVAL;
    }

    return OK;
}

int do_ioctl(struct proc* who, struct message* msg){
    return _sys_ioctl(who, msg->m1_i1, msg->m1_i2, msg->m1_p1);
}


