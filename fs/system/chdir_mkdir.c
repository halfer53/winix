//
// Created by bruce on 27/04/20.
//

#include "../fs.h"

int sys_chdir(struct proc* who, char* pathname){
    int ret = OK;
    inode_t *inode = NULL, *curr_working;

    if((ret = get_inode_by_path(who, pathname, &inode)))
        return ret;

    if(!inode)
        return EEXIST;

    if(!(inode->i_mode & S_IFDIR)){
        ret = ENOTDIR;
        goto error;
    }

    curr_working = who->fp_workdir;
    who->fp_workdir = inode;
    put_inode(curr_working, false);
    return OK;
    error:
    put_inode(inode, false);
    return ret;
}

int sys_mkdir(struct proc* who, char* pathname, mode_t mode){
    char string[DIRSIZ];
    struct inode *lastdir = NULL, *ino = NULL;
    int ret = OK;
    bool is_dirty = false;

    ret = eat_path(who, pathname, &lastdir, &ino, string);
    if(ret)
        return ret;
    if(ino){
        ret = EEXIST;
        goto final;
    }
    ino = alloc_inode(who, lastdir->i_dev);
    if(!ino){
        ret = ENOSPC;
        goto final;
    }
    ino->i_mode = S_IFDIR | (mode & ~who->umask);
    ret = add_inode_to_directory(lastdir, ino, string);
    if(ret){
        release_inode(ino);
        goto final_dir;
    }
    is_dirty = true;

    final:
    put_inode(ino, is_dirty);
    final_dir:
    put_inode(lastdir, is_dirty);
    return ret;
}
