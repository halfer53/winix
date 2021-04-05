//
// Created by bruce on 30/04/20.
//

#include "../fs.h"
#include <sys/unistd.h>

int sys_umask(struct proc* who, mode_t mask){
    mode_t prev = who->umask;
    who->umask = mask & 0x777;
    return prev;
}

int do_umask(struct proc* who, struct message* msg){
    return sys_umask(who, msg->m1_i1);
}

int sys_access(struct proc* who, char* pathname, int mode){
    struct inode *ino = NULL;
    int ret = 0;
    bool has_access;

    ret = get_inode_by_path(who, pathname, &ino);
    if(mode == F_OK){
        if(!ino)
            return ENOENT;
        goto final;
    }

    has_access = has_file_access(who, ino, mode);
    ret = has_access ? OK : EACCES;

    final:
    put_inode(ino, false);
    return ret;
}

int do_access(struct proc* who, struct message* msg){
    char *buf;
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return EFAULT;
    buf = (char *) get_physical_addr(msg->m1_p1, who);
    return sys_access(who, buf, msg->m1_i1);
}

