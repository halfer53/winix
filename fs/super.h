#ifndef _FS_SUPER_H_
#define _FS_SUPER_H_

struct super_block {
    int magic;
    char s_name[32];
    int s_block_inuse;
    int s_inode_inuse;
    int s_free_blocks;
    int s_free_inodes;
    int s_block_size;
    size_t s_inode_size;
    int s_rootnr;    
    
    block_t s_blockmapnr; //block map sector index
    block_t s_inodemapnr; //inode map sector index
    block_t s_inode_tablenr; //inode map sector index
    int s_ninode; //first free inode number
    int s_nblock; //first free block number

    int s_inode_per_block;
    inode_t *s_iroot;
};


#endif