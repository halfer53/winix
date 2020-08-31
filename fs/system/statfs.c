
#include "../fs.h"
#include <sys/statfs.h>

int sys_statfs(struct proc* who, char *path, struct statfs *buf){
    int ret = 0;
    struct superblock *sb;
    inode_t *inode = NULL, *last_dir = NULL;
    char string[DIRSIZ];

    if((ret = eat_path(who, path, &last_dir, &inode, string)))
        return ret;
    if(!last_dir){
        ret = ENOENT;
        goto final;
    }
        
    sb = get_sb(last_dir->i_dev);
    if(!sb){
        ret = ENOSYS;
        goto final;
    }

    memset(buf, 0, sizeof(struct statfs));
    buf->f_type = sb->magic;
    buf->f_bsize = sb->s_block_size;
    #ifdef __wramp__
    buf->f_bsize /= 4;
    #endif
    buf->f_blocks = sb->s_block_inuse + sb->s_free_blocks;
    buf->f_bfree = buf->f_bavail = sb->s_free_blocks;
    buf->f_files = sb->s_inode_inuse + sb->s_free_inodes;
    buf->f_ffree = sb->s_free_inodes;
    buf->f_namelen = DIRNAME_LEN;

    final:
    put_inode(last_dir, false);
    put_inode(inode, false);
    return ret;
}


int do_statfs(struct proc* who, struct message *msg){
    vptr_t *vir_ptr = msg->m1_p1;
    vptr_t *vir_msg = msg->m1_p2;
    if(!is_vaddr_accessible(vir_ptr, who) || !is_vaddr_accessible(vir_msg, who))
        return EFAULT;
    return sys_statfs(who, (char *)get_physical_addr(vir_ptr, who) ,(struct statfs*) get_physical_addr(vir_msg, who) );
}


