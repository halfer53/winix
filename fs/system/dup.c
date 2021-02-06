//
// Created by bruce on 27/04/20.
//

#include "../fs.h"

int sys_dup(struct proc* who, int oldfd){
    int ret, open_slot;
    struct filp* file1, *file2;
    if(!is_fd_opened_and_valid(who, oldfd))
        return EBADF;

    ret = get_fd(who, 0, &open_slot, &file2);
    if(ret)
        return ret;

    file1 = who->fp_filp[oldfd];
    who->fp_filp[open_slot] = file1;
    file1->filp_count += 1;
    return open_slot;
}

int sys_dup2(struct proc* who, int oldfd, int newfd){
    int ret, open_slot;
    struct filp* file1, *file2;
    if(!is_fd_opened_and_valid(who, oldfd))
        return EBADF;

    if(newfd < 0 || newfd >= OPEN_MAX)
        return EBADF;
    
    ret = get_fd(who, 0, &open_slot, &file2);
    if(ret)
        return ret;
    if(who->fp_filp[newfd]){
        ret = sys_close(who, newfd);
        if(ret)
            return ret;
    }

    file1 = who->fp_filp[oldfd];
    if(!file1)
        return EBADF;
    who->fp_filp[newfd] = file1;
    file1->filp_count += 1;
    // KDEBUG(("dup2: %d %d by %s %d, filp dev %s\n", oldfd, newfd, who->name,  who->proc_nr, file1->filp_dev->init_name));
    return newfd;
}

int do_dup(struct proc* who, struct message* msg){
    return sys_dup(who, msg->m1_i1);
}

int do_dup2(struct proc* who, struct message* msg){
    return sys_dup2(who, msg->m1_i1, msg->m1_i2);
}

