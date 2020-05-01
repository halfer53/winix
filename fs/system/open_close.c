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



int _sys_open(struct proc *who, char *path, int flags, mode_t mode, dev_t devid){
    filp_t *filp;
    int i,open_slot,ret;
    inode_t *inode = NULL, *lastdir = NULL;
    char string[DIRSIZ];
    struct device* dev;

    dev = get_dev(devid);
    if(!dev)
        return ENXIO;

    if(ret = eat_path(who, path, &lastdir, &inode, string))
        return ret;

    if(inode && (flags & O_EXCL) && (flags & O_CREAT)){
        ret = EEXIST;
        goto final;
    }


    if(inode && (inode->i_mode & S_IFDIR) && (flags & O_WRONLY)){
        ret = EISDIR;
        goto final;
    }

    if(ret = get_fd(who, 0, &open_slot, &filp))
        goto final;

    if(!inode){

        if(!(flags & O_CREAT)){
            ret = ENOENT;
            goto final;
        }

        if(strlen(string) >= DIRNAME_LEN){
            ret = ENAMETOOLONG;
            goto final;
        }

        if(!(lastdir->i_mode & O_WRONLY))
            return EACCES;

        inode = alloc_inode(who, dev);
        if(!inode){
            ret = ENOSPC;
            goto final;
        }

        if(ret = add_inode_to_directory(lastdir, inode, string)){
            release_inode(inode);
            goto final;
        }
        inode->i_mode = mode & ~who->umask;
    }

    init_filp_by_inode(filp, inode);
    filp->filp_mode = inode->i_mode;
    filp->filp_flags = flags;
    who->fp_filp[open_slot] = filp;
    if(ret = inode->i_dev->fops->open(inode, filp))
        goto final;

    ret = open_slot;
    KDEBUG(("path %s Last dir %d, ret inode %d\n", path, lastdir->i_num, inode->i_num));

    final:
    put_inode(lastdir, false);
    put_inode(inode, false);
    return ret;
}

int sys_creat(struct proc* who, char* path, mode_t mode){
    return sys_open(who, path, O_CREAT | O_EXCL | O_RDWR, mode);
}

int sys_open(struct proc *who, char *path, int flags, mode_t mode){
    return _sys_open(who, path, flags, mode, ROOT_DEV);
}
int sys_mknod(struct proc* who, char *pathname, mode_t mode, dev_t devid){
    return _sys_open(who, pathname, O_CREAT | O_EXCL | O_RDWR, mode, devid);
}