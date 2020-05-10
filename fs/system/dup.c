//
// Created by bruce on 27/04/20.
//

#include "../fs.h"

int sys_dup(struct proc* who, int oldfd){
    int ret, open_slot;
    struct filp* oldfile, *newfile;
    if(!is_fd_opened_and_valid(who, oldfd))
        return EBADF;

    ret = get_fd(who, 0, &open_slot, &newfile);
    if(ret)
        return ret;

    oldfile = who->fp_filp[oldfd];
    who->fp_filp[open_slot] = oldfile;
    oldfile->filp_count += 1;
    return open_slot;
}

int do_dup(struct proc* who, struct message* msg){
    return sys_dup(who, msg->m1_i1);
}

