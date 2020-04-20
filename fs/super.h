#ifndef _FS_SUPER_H_
#define _FS_SUPER_H_ 1

#include <fs/inode.h>

struct superblock {
    int magic;
    char s_name[32];
    int s_block_inuse;
    int s_inode_inuse;
    int s_free_blocks;
    int s_free_inodes;
    int s_block_size;
    int s_inode_size;
    int s_rootnr;    

    block_t s_superblock_nr;
    disk_word_t s_superblock_size;
    block_t s_blockmapnr; // block map block index
    disk_word_t s_blockmap_size;
    block_t s_inodemapnr; // inode map block index
    disk_word_t s_inodemap_size;
    block_t s_inode_tablenr; // inode map block index
    block_t s_inode_table_size;

    int s_ninode; // first free inode number
    int s_nblock; // first free block number

    int s_inode_per_block;
    inode_t *s_iroot;
};


#endif