#include "fs.h"

static filp_t fd_table[NR_FILPS];

int get_fd(proc_t *curr,int start, int *k, filp_t **fpt){
    register int i = -1;
    register filp_t *f;

    for( i=start; i< OPEN_MAX; i++){
        if(curr->fp_filp[i] == NIL_FILP){
            *k = i;
            break;
        }
    }

    if(i==-1)
        return EMFILE;

     for (f = &fd_table[0]; f < &fd_table[NR_FILPS]; f++) {
        if (f->filp_count == 0) {
            // f->filp_mode = bits;
            f->filp_pos = 0L;
            f->filp_flags = 0;
            *fpt = f;
            return(OK);
        }
    }

    return(ENFILE);
}


filp_t *get_filp(int fd){
    return &fd_table[fd];
}

filp_t *find_filp(inode_t *inode){
    int i;
    for(i = 0; i< FILPNR; i++){
        if(fd_table[i].filp_ino == inode){
            return &fd_table[i];
        }
    }
    return NULL;
}
