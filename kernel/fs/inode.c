#include "fs.h"

extern inode_t inode_table[NR_INODES];

inode_t* get_inode(int num){
    register inode_t* rep;
    register int i=0;
    for(i = 0; i < NR_INODES; i++ ){
        rep = &inode_table[i];
        if(rep->i_num == num)
            return rep;
    }
    return NULL;

}


int put_inode(inode_t *inode){
    
}