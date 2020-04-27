//
// Created by bruce on 27/04/20.
//

#include "../fs.h"

int sys_chown(struct proc* who, const char *pathname, uid_t owner, gid_t group){
    return OK;
}

int sys_chmod(struct proc* who, const char *pathname, mode_t mode){
    filp_t *filp;
    int open_slot,ret;
    inode_t *inode = NULL, *lastdir = NULL, *curr_working;
    char string[DIRSIZ];

    if(ret = eat_path(who, pathname, &lastdir, &inode, string))
        return ret;

    if(!inode)
        return ENOENT;

    inode->i_mode = mode;
    put_inode(lastdir, false);
    put_inode(inode, true);
    return OK;
}

