#include <fs/fs.h>



int sys_read(struct proc *who,int fd, void *buf, size_t count){
    struct filp* file;
    SET_CALLER(current_proc);
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    file = who->fp_filp[fd];
    return file->filp_dev->fops->read(file, buf, count, file->filp_pos);
}

int sys_write(struct proc *who,int fd, void *buf, size_t count){
    struct filp* file;
    SET_CALLER(current_proc);
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    file = who->fp_filp[fd];
    return file->filp_dev->fops->write(file, buf, count, file->filp_pos);
}

int do_read(struct proc* who, struct message* msg){
    char* buf = (char *) get_physical_addr(msg->m1_p1, who);
    return sys_read(who, msg->m1_i1, buf, msg->m1_i2);
}

int do_write(struct proc* who, struct message* msg){
    char* buf = (char *) get_physical_addr(msg->m1_p1, who);
    return sys_read(who, msg->m1_i1, buf, msg->m1_i2);
}


