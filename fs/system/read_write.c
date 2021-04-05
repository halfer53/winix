#include "../fs.h"
#include <winix/sys_stdio.h>

int sys_read(struct proc *who,int fd, void *buf, size_t count){
    struct filp* file;
    SET_CALLER(who);
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    file = who->fp_filp[fd];
    // kprintf("call %d, fops %p\n", file->filp_dev->dev_id, file->filp_dev->fops->read);
    return file->filp_dev->fops->read(file, buf, count, file->filp_pos);
}

int sys_write(struct proc *who,int fd, void *buf, size_t count){
    struct filp* file;
    int ret;
    SET_CALLER(who);
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    file = who->fp_filp[fd];
    // KDEBUG(("write fd %d for dev %s\n", fd, file->filp_dev->init_name));
    ret = file->filp_dev->fops->write(file, buf, count, file->filp_pos);
    if(ret > 0)
        file->filp_ino->i_mtime = get_unix_time();
    return ret;
}

int do_read(struct proc* who, struct message* msg){
    char* buf = (char *) get_physical_addr(msg->m1_p1, who);
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return EFAULT;
    return sys_read(who, msg->m1_i1, buf, msg->m1_i2);
}

int do_write(struct proc* who, struct message* msg){
    char* buf = (char *) get_physical_addr(msg->m1_p1, who);
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return EFAULT;
    return sys_write(who, msg->m1_i1, buf, msg->m1_i2);
}


