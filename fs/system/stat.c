//
// Created by bruce on 27/04/20.
//
#include "../fs.h"

int sys_stat(struct proc* who, char *pathname, struct stat *statbuf){
    int ret;
    inode_t *inode = NULL, *lastdir = NULL;
    struct stat val;
    char string[DIRSIZ];

    if(ret = eat_path(who, pathname, &lastdir,  &inode, string))
        return ret;

    if(!inode)
        return ENOENT;

    val.st_dev = lastdir->i_dev->dev_id;
    val.st_ino = inode->i_num;
    val.st_mode = inode->i_mode;
    val.st_nlink = inode->i_nlinks;
    val.st_uid = inode->i_uid;
    val.st_gid = inode->i_gid;
    val.st_rdev = inode->i_dev->dev_id;
    val.st_size = inode->i_size;
    val.st_blksize = BLOCK_SIZE;
    val.st_blocks = get_inode_blocks(inode);
    val.st_atime = inode->i_atime;
    val.st_mtime = inode->i_mtime;
    val.st_ctime = inode->i_ctime;
    memcpy(statbuf, &val, sizeof(struct stat));
    put_inode(lastdir, false);
    put_inode(inode, false);
    return OK;
}