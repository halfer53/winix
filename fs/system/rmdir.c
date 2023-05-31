#include <fs/fs.h>
#include <fs/cache.h>

int sys_rmdir(struct proc* who, const char* path){
    struct inode* inode = NULL;
    struct dirent_iterator iter;
    struct winix_dirent* curr;
    int ret = 0;
    if(!path)
        return -EFAULT;
    
    if ((ret = get_inode_by_path(who, path, &inode)) < 0)
        return ret;
    
    if(!S_ISDIR(inode->i_mode)){
        ret = -ENOTDIR;
        goto final;
    }
    
    // if(!permission(dir, W_OK))
    //     return -EACCES;
    
    iter_dirent_init(&iter, inode, 0, 0);
    while(iter_dirent_has_next(&iter)){
        curr = iter_dirent_get_next(&iter);
        if(char32_strcmp(curr->dirent.d_name, "..") != 0 && char32_strcmp(curr->dirent.d_name, ".") != 0 
            && curr->dirent.d_name[0] != '\0' ){
            
            iter_dirent_close(&iter);
            ret = -ENOTEMPTY;
            goto final;
        }
    }

    iter_dirent_close(&iter);
    put_inode(inode, false);
    
    ret = sys_unlink(who, path);    
    goto result;
    
final:
    if (inode)
        put_inode(inode, false);
result:
    return ret;
}

int do_rmdir(struct proc* who, struct message* m){
    char path[PATH_MAX];
    int ret;
    if(copy_from_user(who, path, m->m1_p1, PATH_MAX) < 0)
        return -EFAULT;
    
    ret = sys_rmdir(who, path);
    return ret;
}