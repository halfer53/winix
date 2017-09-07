#include "fs.h"

int sys_open(struct proc *who, char *path, mode_t mode){
    filp_t *filp;
    int i,open_slot;
    int ret;
    inode_t *inode = eat_path(path);


    if(inode == NIL_INODE && mode && O_CREAT){
        inode = alloc_inode();
    }

    ret = get_fd(who, 0, &open_slot, &filp);
    who->fp_filp[open_slot] = filp;

    return open_slot;
}



