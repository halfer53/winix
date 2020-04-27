#include "../fs.h"


 int sys_close(struct proc *who,int fd){
     filp_t *filp;
     int ret;
     if(!is_fd_opened_and_valid(who, fd))
        return EINVAL;

     filp = who->fp_filp[fd];
     filp->filp_count -= 1;
     if(filp->filp_count == 0){
         put_inode(filp->filp_ino, true);
     }
     ret = filp->filp_dev->fops->close(filp->filp_ino, filp);
     if(ret)
         return ret;
     who->fp_filp[fd] = NULL;
     return OK;
 }

 inode_t* new_inode_by_dir(struct proc *who, struct inode* dir, char* string){
    inode_t* inode = alloc_inode(dir->i_dev);
//    inode->i_mode = S_IFREG;
    inode->i_gid = who->gid;
    inode->i_uid = who->uid;

    add_inode_to_directory(dir, inode, string);
    return inode;
}

int sys_creat(struct proc* who, char* path, mode_t mode){
    
}

int sys_open(struct proc *who, char *path, int flags, mode_t mode){
    filp_t *filp;
    int i,open_slot,ret;
    inode_t *inode, *lastdir;
    char string[DIRSIZ];
    int inum;

    inode = eat_path(who, path, &lastdir, string);
    if(inode == NIL_INODE){
        if(flags & O_CREAT){
            inode = new_inode_by_dir(who, lastdir, string);
        }else {
            return EINVAL;
        }
    }
    ret = get_fd(who, 0, &open_slot, &filp);
    if(ret)
        return ret;

    init_filp_by_inode(filp, inode);
    filp->filp_mode = mode;
    filp->filp_flags = flags;
    who->fp_filp[open_slot] = filp;
    ret = inode->i_dev->fops->open(inode, filp);
    if(ret)
        return ret;

    inum = inode->i_num;
    KDEBUG(("path %s Last dir %d, ret inode %d\n", path, lastdir->i_num, inum));
    put_inode(lastdir, false);
    return open_slot;
}