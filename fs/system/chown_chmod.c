//
// Created by bruce on 27/04/20.
//

#include "../fs.h"

int sys_chown(struct proc* who,  char *pathname, uid_t owner, gid_t group){
    return EINVAL;
}

int do_chown(struct proc* who, struct message* msg){
   char* path = (char *) get_physical_addr(msg->m1_p1, who);
   if(!is_vaddr_accessible(msg->m1_p1, who))
       return EFAULT;
   return sys_chown(who, path, msg->m1_i1, msg->m1_i2);
}

int sys_chmod(struct proc* who, char *pathname, mode_t mode){
    int ret;
    inode_t *inode = NULL;

    if((ret = get_inode_by_path(who, pathname, &inode)))
        return ret;

    if(!inode)
        return ENOENT;

    inode->i_mode = mode;
    put_inode(inode, true);
    return OK;
}

int do_chmod(struct proc* who, struct message* msg){
   char* path = (char *) get_physical_addr(msg->m1_p1, who);
   if(!is_vaddr_accessible(msg->m1_p1, who))
       return EFAULT;
   return sys_chmod(who, path, msg->m1_i1);
}

