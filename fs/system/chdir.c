//
// Created by bruce on 27/04/20.
//

#include "../fs.h"

int sys_chdir(struct proc* who, char* pathname){
    filp_t *filp;
    int open_slot,ret;
    inode_t *inode = NULL, *lastdir = NULL, *curr_working;
    char string[DIRSIZ];

    if(ret = eat_path(who, pathname, &lastdir, &inode, string))
        return ret;

    if(!inode)
        return EEXIST;

    if(!(inode->i_mode & S_IFDIR))
        return ENOTDIR;

    curr_working = who->fp_workdir;
    put_inode(curr_working, false);
    put_inode(lastdir, false);
    who->fp_workdir = inode;
    return OK;
}
