#include "fs.h"
#include <winix/list.h>

static filp_t fd_table[NR_FILPS];

int get_fd(struct proc *curr, int start, int *open_slot, filp_t **fpt){
    int i, idx_bak;
    bool found = false;
    filp_t *f;

    for( i=start; i< OPEN_MAX; i++){
        if(curr->fp_filp[i] == NIL_FILP){
            *open_slot = i;
            found = true;
            break;
        }
    }

    if(!found)
        return EMFILE;

    for(i = 0; i < NR_FILPS; i++ ){
        f = &fd_table[i];
        if (f->filp_count == 0) {
            memset(f,0, sizeof(struct filp));
            f->filp_table_index = i;
            *fpt = f;
            return(OK);
        }
    }
    return ENFILE;
}

int init_filp_by_inode(struct filp* filp, struct inode* inode){
    filp->filp_ino = inode;
    filp->filp_count += 1;
    filp->filp_dev = inode->i_dev;
    return OK;
}

int release_filp(struct filp* file){
    file->filp_count = 0;
    return 0;
}

filp_t *find_filp(inode_t *inode){
    int i;
    for(i = 0; i< NR_FILPS; i++){
        if(fd_table[i].filp_ino == inode){
            return &fd_table[i];
        }
    }
    return NULL;
}

filp_t *get_free_filp(){
    filp_t* rep;
    int i;
    for(i = 0; i < NR_FILPS; i++ ){
        rep = &fd_table[i];
        if(rep->filp_count == 0){
            return rep;
        }
    }
    return NULL;
}

void init_filp(){
    filp_t* rep;
    int i = 0;
    for(i = 0; i < NR_FILPS; i++ ){
        rep = &fd_table[i];
        rep->filp_count = 0;
        rep->filp_table_index = i;
        i++;
    }
}
