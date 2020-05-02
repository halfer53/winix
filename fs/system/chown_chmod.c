//
// Created by bruce on 27/04/20.
//

#include "../fs.h"

int sys_chown(struct proc* who, const char *pathname, uid_t owner, gid_t group){
    return OK;
}

int sys_chmod(struct proc* who, const char *pathname, mode_t mode){
    int ret;
    inode_t *inode = NULL;

    if((ret = get_inode_by_path(who, pathname, &inode)))
        return ret;

    if(!inode)
        return ENOENT;

    inode->i_mode = mode;
    put_inode(inode, true);
    return OK;
}

