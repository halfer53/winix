#include <fs/fs.h>
#include <kernel/clock.h>

int filp_read(struct proc* who, struct filp* file, void *buf, size_t count){
    int ret;
    SET_CALLER(who);
    ret = file->filp_dev->fops->read(file, buf, count, file->filp_pos);
    return ret;
}

int sys_read(struct proc *who,int fd, void *buf, size_t count){
    struct filp* file;
    int ret;
    if(!is_fd_opened_and_valid(who, fd))
        return -EBADF;
    file = who->fp_filp[fd];
    ret = filp_read(who, file, buf, count);
    // KDEBUG(("read ret %d, call %d, fops %p\n", ret, file->filp_dev->dev_id, (void *)file->filp_dev->fops->read));
    return ret;
}

int filp_write(struct proc* who, struct filp* file, void *buf, size_t count){
    int ret;
    SET_CALLER(who);
    ret = file->filp_dev->fops->write(file, buf, count, file->filp_pos);
    return ret;
}

int sys_write(struct proc *who,int fd, void *buf, size_t count){
    struct filp* file;
    int ret;
    
    if(!is_fd_opened_and_valid(who, fd))
        return -EBADF;
    file = who->fp_filp[fd];
    // KDEBUG(("write fd %d for dev %s\n", fd, file->filp_dev->init_name));
    ret = filp_write(who, file, buf, count);
    if(ret > 0)
        file->filp_ino->i_mtime = get_unix_time();
    return ret;
}

int do_read(struct proc* who, struct message* msg){
    char* buf = (char *) get_physical_addr(msg->m1_p1, who);
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return -EFAULT;
    return sys_read(who, msg->m1_i1, buf, msg->m1_i2);
}

int do_write(struct proc* who, struct message* msg){
    char* buf = (char *) get_physical_addr(msg->m1_p1, who);
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return -EFAULT;
    return sys_write(who, msg->m1_i1, buf, msg->m1_i2);
}


