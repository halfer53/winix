#ifndef _FS_SUPER_H_
#define _FS_SUPER_H_ 1

#include <fs/inode.h>

struct superblock {
    unsigned int magic;
    unsigned int s_block_inuse;
    unsigned int s_inode_inuse;
    unsigned int s_free_blocks;
    unsigned int s_free_inodes;
    unsigned int s_block_size;
    unsigned int s_inode_size;
    unsigned int s_rootnr;

    block_t s_superblock_nr;
    unsigned int s_superblock_size;
    block_t s_blockmapnr; // block map block index
    unsigned int  s_blockmap_size;
    block_t s_inodemapnr; // inode map block index
    unsigned int  s_inodemap_size;
    block_t s_inode_tablenr; // inode map block index
    block_t s_inode_table_size;

    int s_ninode; // first free inode number
    int s_nblock; // first free block number

    int s_inode_per_block;
    inode_t *s_iroot;
    char32_t s_name[32];
};


#endif
