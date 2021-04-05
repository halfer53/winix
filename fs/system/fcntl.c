//
// Created by bruce on 27/04/20.
//

#include "../fs.h"

int __sys_fcntl(struct proc* who, int fd, int cmd, void* arg){
    struct filp* file;
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    file = who->fp_filp[fd];
    switch (cmd)
    {
    case F_GETFL:
        return file->filp_flags;
    case F_SETFL:
        file->filp_flags = *((int*)arg + (int)who->ctx.rbase); 
        break;

    default:
        return EINVAL;
    }
    return OK;
}

int do_fcntl(struct proc* who, struct message* msg){
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return EFAULT;
    return __sys_fcntl(who, msg->m1_i1, msg->m1_i2, msg->m1_p1);
}


