//
// Created by bruce on 14/05/20.
//

#include <fs/fs.h>


int sys_mknod(struct proc* who, const char *path, mode_t mode, dev_t devid){
    int ret;
    inode_t *inode = NULL, *lastdir = NULL;
    char string[WINIX_NAME_LEN];
    struct device* dev;

    dev = get_dev(devid);
    if(!dev)
        return -ENXIO;

    if((ret = eat_path(who, path, &lastdir, &inode, string)))
        return ret;

    if(inode || *string == '\0'){
        ret = -EEXIST;
        goto final;
    }

    inode = alloc_inode(lastdir->i_dev, dev);
    if(!inode){
        ret = -ENOSPC;
        goto final;
    }
    
    if((ret = add_inode_to_directory(who, lastdir, inode, string))){
        release_inode(inode);
        goto final;
    }
    init_inode_proc_field(inode, who, dev->device_type, mode);

    final:
    put_inode(inode, false);
    put_inode(lastdir, ret == 0);
    return ret;
}

int do_mknod(struct proc* who, struct message* msg){
    if(!is_vaddr_accessible(msg->m1_p1, who))
        return -EFAULT;
    return sys_mknod(who, (char*)get_physical_addr(msg->m1_p1, who),
        msg->m1_i1, msg->m1_i2);
}


