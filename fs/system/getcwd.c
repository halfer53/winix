#include "../fs.h"
#include <fs/path.h>

int sys_getcwd(struct proc* who, char* pathname, int size){
    int ret = OK, len;
    inode_t *inode, *parent_inode;
    int inum;
    char string[NAME_MAX];
    char *p;

    if(!is_vaddr_accessible(pathname, who) || !is_vaddr_accessible(pathname + size, who))
        return EFAULT;
    if(size <= 1)
        return ERANGE;

    p = (char *)get_physical_addr(pathname+ size, who);
    *p = '\0';
    len = 1;
    inode = who->fp_workdir;
    inode->i_count += 1;

    while(inode->i_num != ROOT_INODE_NUM){
        inum = get_parent_inode_num(inode);
        parent_inode = get_inode(inum, inode->i_dev);
        ret = get_child_inode_name(parent_inode, inode, string);
        // KDEBUG(("cwd: ret %d curr %d (%s), parent %d\n", ret, inode->i_num, string, inum));
        if(ret == ERR){
            // KDEBUG(("corruptted fs of %d inode\n", inum));
            ret = EFAULT;
            goto end;
        }
        put_inode(inode, false);
        inode = parent_inode;

        len += (ret + 1); // inclusing slash
        if(len >= size){
            ret = ERANGE;
            goto end;
        }
        *--p = '/';
        p -= (ret);
        memcpy(p, string, ret);

    }
    *--p = '/';
    ret = (int)get_virtual_addr(p, who);

end:
    put_inode(inode, false);
    return ret;
}


int do_getcwd(struct proc* who, struct message* m){
    return sys_getcwd(who, m->m1_p1, m->m1_i1);
}
