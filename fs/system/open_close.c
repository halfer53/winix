#include "../fs.h"
#include <kernel/clock.h>

int sys_close(struct proc *who, int fd)
{
    filp_t *filp;
    int ret;
    if (!is_fd_opened_and_valid(who, fd))
        return EBADF;

    filp = who->fp_filp[fd];

    ret = filp->filp_dev->fops->close(filp->filp_dev, filp);
    if (ret)
        return ret;
    who->fp_filp[fd] = NULL;
    return OK;
}

int alloc_inode_under_dir(struct proc* who, struct device* dev, inode_t** _inode, inode_t* lastdir, char string[DIRSIZ]){
    inode_t* inode;
    int ret;
    dev = lastdir->i_dev;
    inode = alloc_inode(who, dev, dev);
    if (!inode)
        return ENOSPC;
    
    if ((ret = add_inode_to_directory(who, lastdir, inode, string)))
    {
        release_inode(inode);
        return ret;
    }
    *_inode = inode;
    return OK;
}

filp_t* open_filp(struct proc* who, char *path, int flags, mode_t mode){

}

int sys_open(struct proc *who, char *path, int flags, mode_t mode)
{
    filp_t *filp;
    int open_slot, ret;
    inode_t *inode = NULL, *lastdir = NULL;
    char string[DIRSIZ];
    struct device *dev;
    clock_t unix_time = get_unix_time();
    bool is_new = false;

    if ((ret = eat_path(who, path, &lastdir, &inode, string)))
        return ret;

    if (inode && (flags & O_EXCL) && (flags & O_CREAT))
    {
        ret = EEXIST;
        goto final;
    }

    if (inode && (inode->i_mode & S_IFDIR) && (flags & O_WRONLY))
    {
        ret = EISDIR;
        goto final;
    }
    dev = lastdir->i_dev;
    if(!inode){
        if (*string){ // if there still components left in path
            if (!(flags & O_CREAT))
            {
                ret = ENOENT;
                goto final;
            }
            if (ret = alloc_inode_under_dir(who, dev, &inode, lastdir, string))
                goto final;
            inode->i_mode = dev->device_type | (mode & ~(who->umask));
            is_new = true;

        }else{ // if no components left, then path refers to the directory
            // if opening a directory with write access, EISDIR is returned
            if (flags & O_WRONLY)
            {
                ret = EISDIR;
                goto final;
            }
            inode = lastdir;
        }
    }
    filp = get_free_filp();
    if (!filp)
        return ENFILE;
        
    if ((ret = get_fd(who, 0, &open_slot, filp)))
        goto final;

    if(flags & O_TRUNC)
        truncate_inode(inode);

    init_filp_by_inode(filp, inode);
    
    if(flags & O_APPEND)
        filp->filp_pos = inode->i_size;
    
    filp->filp_mode = mode;
    filp->filp_flags = flags;
    who->fp_filp[open_slot] = filp;
    inode->i_atime = unix_time;

    if ((ret = inode->i_dev->fops->open(inode->i_dev, filp)))
        goto final;

    ret = open_slot;
    // KDEBUG(("Open: path %s Last dir %d, ret inode %d\n", path, lastdir->i_num, inode->i_num));

final:
    put_inode(lastdir, is_new);
    return ret;
}

int sys_creat(struct proc *who, char *path, mode_t mode)
{
    return sys_open(who, path, O_CREAT | O_EXCL, mode);
}

int do_open(struct proc *who, struct message *msg)
{
    char *path;
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return EFAULT;
    path = (char *)get_physical_addr(msg->m1_p1, who);
    return sys_open(who, path, msg->m1_i1, msg->m1_i2);
}

int do_creat(struct proc *who, struct message *msg)
{
    char *path;
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return EFAULT;
    path = (char *)get_physical_addr(msg->m1_p1, who);
    return sys_creat(who, path, msg->m1_i1);
}

int do_close(struct proc *who, struct message *msg)
{
    return sys_close(who, msg->m1_i1);
}
