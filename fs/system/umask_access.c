//
// Created by bruce on 30/04/20.
//

#include "../fs.h"
#include <sys/unistd.h>

int sys_umask(struct proc* who, mode_t mask){
    who->umask = mask & 0x777;
    return OK;
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
    char* buf = (char *) get_physical_addr(msg->m1_p1, who);
    return sys_access(who, buf, msg->m1_i1);
}

