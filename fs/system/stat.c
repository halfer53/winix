//
// Created by bruce on 27/04/20.
//
#include "../fs.h"

void set_statbuf(struct inode* inode, struct stat* statbuf){
    statbuf->st_dev = inode->i_dev->dev_id;
    statbuf->st_ino = inode->i_num;
    statbuf->st_mode = inode->i_mode;
    statbuf->st_nlink = inode->i_nlinks;
    statbuf->st_uid = inode->i_uid;
    statbuf->st_gid = inode->i_gid;
    statbuf->st_rdev = inode->i_dev->dev_id;
    statbuf->st_size = inode->i_size; 
#ifdef __wramp__
    statbuf->st_size *= 4; //convert words to bytes
#endif
    statbuf->st_blksize = BLOCK_SIZE;
    statbuf->st_blocks = get_inode_blocks(inode);
    statbuf->st_atime = inode->i_atime;
    statbuf->st_mtime = inode->i_mtime;
    statbuf->st_ctime = inode->i_ctime;
}

int sys_fstat(struct proc* who, int fd, struct stat* statbuf){
    int ret;
    struct filp* file;
    if(!is_fd_opened_and_valid(who, fd))
        return EBADF;
    file = who->fp_filp[fd];
    set_statbuf(file->filp_ino, statbuf);
    return OK;
}

int sys_stat(struct proc* who, char *pathname, struct stat *statbuf){
    int ret;
    inode_t *inode = NULL;

    if((ret = get_inode_by_path(who, pathname, &inode)))
        return ret;

    if(!inode)
        return ENOENT;

    set_statbuf(inode, statbuf);
    put_inode(inode, false);
    return OK;
}

int do_stat(struct proc* who, struct message* msg){
    if(!is_vaddr_accessible(msg->m1_p1, who) 
        || !is_vaddr_accessible(msg->m1_p2, who)){
            return EFAULT;
        }
    return sys_stat(who, (char*)get_physical_addr(msg->m1_p1, who),
        (struct stat*)get_physical_addr(msg->m1_p2, who));
}

int do_fstat(struct proc* who, struct message* msg){
    if(!is_vaddr_accessible(msg->m1_p1, who) ){
            return EFAULT;
        }
    return sys_fstat(who, msg->m1_i1,
        (struct stat*)get_physical_addr(msg->m1_p1, who));
}
