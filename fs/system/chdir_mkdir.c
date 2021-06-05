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

    if(!(S_ISDIR(inode->i_mode))){
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
    block_t bnr;

    ret = eat_path(who, pathname, &lastdir, &ino, string);
    if(ret)
        return ret;
    if(ino){
        ret = EEXIST;
        goto final;
    }
    ino = alloc_inode(who, lastdir->i_dev, lastdir->i_dev);
    if(!ino){
        ret = ENOSPC;
        goto final;
    }
    ino->i_mode = S_IFDIR | (mode & ~(who->umask));
    ino->i_zone[0] = alloc_block(ino, lastdir->i_dev);
    ino->i_size = BLOCK_SIZE;
    init_dirent(lastdir, ino);
    ret = add_inode_to_directory(who, lastdir, ino, string);
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

int do_chdir(struct proc* who, struct message* msg){
    char* path = (char *) get_physical_addr(msg->m1_p1, who);
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return EFAULT;
    return sys_chdir(who, path);
}

int do_mkdir(struct proc* who, struct message* msg){
    char* path = (char *) get_physical_addr(msg->m1_p1, who);
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return EFAULT;
    return sys_mkdir(who, path, msg->m1_i1);
}
