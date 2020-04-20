#include "fs.h"


 int sys_close(struct proc *who,int fd){
     filp_t *filp;
     if(!is_fd_opened_and_valid(who, fd))
        return EINVAL;

     filp = who->fp_filp[fd];
     filp->filp_count -= 1;
     if(filp->filp_count == 0){
         filp->filp_ino->i_count -= 1;
         put_inode(filp->filp_ino, filp->filp_ino->i_dev);
     }
     who->fp_filp[fd] = NIL_FILP;
     return OK;
 }

 inode_t* new_inode_by_dir(struct proc *who, struct inode* dir, char* string){
    inode_t* inode = alloc_inode(dir->i_dev);
    inode->i_mode = S_IFREG;
    inode->i_gid = who->gid;
    inode->i_uid = who->uid;
    inode->i_nlinks = 1;
    add_inode_to_directory(dir, inode, string);
    return inode;
}

int sys_open(struct proc *who, char *path, mode_t mode){
    filp_t *filp;
    int i,open_slot;
    int ret;
    inode_t *inode, *lastdir;
    char string[DIRSIZ];
    inode = eat_path(path, &lastdir, string);
    int inum;

    if(inode == NIL_INODE){
        if(mode & O_CREAT){
            inode = new_inode_by_dir(who, lastdir, string);
        }else {
            return EINVAL;
        }
    }

    ret = get_fd(who, 0, &open_slot, &filp);
    if(ret)
        return ret;

    filp->filp_table_index = open_slot;
    filp->filp_ino = inode;
    inode->i_count += 1;
    filp->filp_count += 1;
    filp->filp_mode = mode;
    who->fp_filp[open_slot] = filp;

    inum = inode->i_num;
    XDEBUG(("Last dir %d, ret inode %d\n", lastdir->i_num, inum));
    return open_slot;
}