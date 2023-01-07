#include <fs/fs.h>
#include <fs/path.h>

int sys_getcwd(struct proc* who, char* pathname, int size, char** result){
    int ret = 0, len, ret2;
    inode_t *inode, *parent_inode;
    int inum;
    char string[WINIX_NAME_LEN];
    char *p;

    
    if(size <= 1)
        return -ERANGE;

    p = pathname + size - 1;
    *p = '\0';
    len = 1;
    inode = who->fp_workdir;
    inode->i_count += 1;

    while(inode->i_num != ROOT_INODE_NUM){
        inum = get_parent_inode_num(inode);
        parent_inode = get_inode(inum, inode->i_dev);
        ret2 = get_child_inode_name(parent_inode, inode, string);
        // kdebug("cwd: ret %d curr %d (%s), parent %d\n", ret, inode->i_num, string, inum);
        if(ret2 < 0){
            ret = ret2;
            goto end;
        }

        put_inode(inode, false);
        inode = parent_inode;

        len += (ret2 + 1); // inclusing slash
        if(len >= size){
            ret = -ERANGE;
            goto end;
        }
        *--p = '/';
        p -= (ret2);
        memcpy(p, string, ret2);

    }
    *--p = '/';
end:
    put_inode(inode, false); 
    *result = p;
    return ret;
}


int do_getcwd(struct proc* who, struct message* m){
    char *p, *path;
    int size = m->m1_i1, ret;
    if(!is_vaddr_ok(m->m1_p1, size, who) )
        return -EFAULT;
    path = (char *)get_physical_addr(m->m1_p1, who);
    ret = sys_getcwd(who, path, size, &p);
    if (ret)
        return ret;
    return (int)(get_virtual_addr(p, who) - (vptr_t*)0);
}
