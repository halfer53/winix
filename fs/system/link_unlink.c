//
// Created by bruce on 28/04/20.
//

#include "../fs.h"

int sys_link(struct proc* who, char *oldpath, char *newpath){
    struct inode* oldinode = NULL, *newinode = NULL, *lastdir = NULL;
    char string[DIRSIZ];
    int ret;

    if((ret = get_inode_by_path(who, oldpath, &oldinode)))
        goto failed_1;

    if((ret = eat_path(who, newpath, &lastdir, &newinode, string)))
        goto final;

    if(newinode){
        ret = EEXIST;
        goto final;
    }

    ret = add_inode_to_directory(who, lastdir, oldinode, string);

    final:
    if(newinode)
        put_inode(newinode, false);
    if(lastdir)
        put_inode(lastdir, ret == OK);

    failed_1:
    if(oldinode)
        put_inode(oldinode, false);
    return ret;
}

int sys_unlink(struct proc* who, char *path){
    char string[DIRSIZ];
    struct inode* lastdir = NULL, *ino = NULL;
    int ret;

    ret = eat_path(who, path, &lastdir, &ino, string);
    if(ret)
        goto final;

    if(!ino){
        ret = ENOENT;
        goto final;
    }

    if(S_ISDIR(ino->i_mode)){
        ret = EISDIR;
        goto final;
    }

    ret = remove_inode_from_dir(who, lastdir, ino, string);
    put_inode(ino, false);
    
    if(ino->i_nlinks == 0 && ino->i_count == 0){
        ret = release_inode(ino);
    }
    goto final;

    inode_err:
    put_inode(ino, false);
    
    final:
    put_inode(lastdir, false);
    return ret;
}

int do_link(struct proc* who, struct message* msg){
    if(!is_vaddr_accessible(msg->m1_p1, who) 
        || !is_vaddr_accessible(msg->m1_p2, who)){
            return EFAULT;
        }
    return sys_link(who, (char*)get_physical_addr(msg->m1_p1, who),
        (char*)get_physical_addr(msg->m1_p2, who));
}

int do_unlink(struct proc* who, struct message* msg){
    if(!is_vaddr_accessible(msg->m1_p1, who) ){
            return EFAULT;
        }
    return sys_unlink(who, (char*)get_physical_addr(msg->m1_p1, who));
}



