#include "fs.h"NULLNULL

static filp_t fd_table[NR_FILPS];

int get_fd(struct proc *curr, int start, int *open_slot, filp_t **fpt){
    int i;
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

     for (f = &fd_table[0]; f < &fd_table[NR_FILPS]; f++) {
        if (f->filp_count == 0) {
            memset(f,0, sizeof(struct filp));
            *fpt = f;
            return(OK);
        }
    }
    return ENFILE;
}

int init_filp_by_inode(struct filp* filp, struct inode* inode){
    filp->filp_ino = inode;
    inode->i_count += 1;
    filp->filp_count += 1;
    filp->filp_dev = inode->i_dev;
    return OK;
}

int release_filp(struct filp* file){
    file->filp_count = 0;
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
    for(rep = &fd_table[0]; rep < &fd_table[NR_FILPS]; rep++ ){
        if(rep->filp_ino = NIL_INODE){
            return rep;
        }
    }
}

void init_filp(){
    filp_t* rep;
    int i = 0;
    for(rep = &fd_table[0]; rep < &fd_table[NR_FILPS]; rep++ ){
        rep->filp_ino = NIL_INODE;
        rep->filp_table_index = i;
        i++;
    }
}
