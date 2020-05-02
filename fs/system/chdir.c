//
// Created by bruce on 27/04/20.
//

#include "../fs.h"

int sys_chdir(struct proc* who, char* pathname){
    int ret;
    inode_t *inode = NULL, *curr_working;

    if((ret = get_inode_by_path(who, pathname, &inode)))
        return ret;

    if(!inode)
        return EEXIST;

    if(!(inode->i_mode & S_IFDIR))
        return ENOTDIR;

    curr_working = who->fp_workdir;
    put_inode(curr_working, false);
    who->fp_workdir = inode;
    return OK;
}
