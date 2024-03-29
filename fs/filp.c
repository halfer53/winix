#include <fs/fs.h>
#include <winix/list.h>

static filp_t fd_table[NR_FILPS];

int get_fd(struct proc *curr, int start, int *open_slot, filp_t *fpt){
    int i;
    bool found = false;

    for( i=start; i< OPEN_MAX; i++){
        if(curr->fp_filp[i] == NIL_FILP){
            *open_slot = i;
            found = true;
            break;
        }
    }

    if(!found)
        return -EMFILE;

    fpt->filp_table_index = i;
    return 0;
}

int init_filp_by_inode(struct filp* filp, struct inode* inode){
    filp->filp_ino = inode;
    filp->filp_count += 1;
    filp->filp_dev = inode->i_dev;
    return 0;
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
            memset(rep, 0, sizeof(filp_t));
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
    }
}
