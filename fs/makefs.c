#include "fs.h"
#include <stdio.h>
#include <string.h>

//struct superblock superblock = {
//        0xabcdefab, // magic
//        "WINIX_ROOTFS", // name
//        65, // blocks in use
//        1, // inode in use
//        16319, // free blocks
//        495, // free inodes
//        1024, // block size
//        128, // inode size
//        1, // root inode number
//        1, // block bitmap block index
//        2, // inode bitmap block index
//        3, // inode table block index
//        2, // first free inode number
//        67, // first free block number
//        8, // inode per block
//        NULL, // root inode
//};

unsigned int setBits(unsigned int n)
{
    unsigned int ret = 0;
    bitmap_set_nbits(&ret, 1,0,n);
    return ret;
}

int makefs( disk_word_t* disk_raw, size_t disk_size_words)
{
    disk_word_t *pdisk = disk_raw;
    struct dirent* pdir;
    struct dirent d1, d2;
    const time_t now = 1587268254;
    const int root_inode_num = 1;
    inode_t root_node;
    disk_word_t blocks_nr = disk_size_words / 1024;
    disk_word_t disk_size_in_bytes = disk_size_words * 4;
    int total_blocks = blocks_nr;
    block_t sb_block_nr = 0;
    block_t blockmap_block_nr = 1;
    block_t inodemap_block_nr = 2;
    block_t inode_table_block_nr = 3;
    block_t inode_tablesize = (int)(blocks_nr * 0.05) * BLOCK_SIZE;
    block_t root_node_block_nr = inode_table_block_nr + (inode_tablesize / BLOCK_SIZE);
    block_t first_free_block = root_node_block_nr + 1;
    block_t block_in_use = root_node_block_nr + 1;
    block_t remaining_blocks = blocks_nr - block_in_use;
    disk_word_t remaining_words = remaining_blocks * BLOCK_SIZE;
    int inode_per_block = BLOCK_SIZE / INODE_STRUCT_SIZE;
    int free_inodes = inode_tablesize / INODE_STRUCT_SIZE - 1;
    unsigned int bitval = 0;

    struct superblock superblock = {
            0xabcdefab, // magic
        "WINIX_ROOTFS", // name
        block_in_use, // blocks in use
        1, // inode in use
            remaining_blocks, // free blocks
        free_inodes, // free inodes
        BLOCK_SIZE, // block size
        INODE_STRUCT_SIZE, // inode size
            root_inode_num, // root inode number

        sb_block_nr,
        BLOCK_SIZE,
            blockmap_block_nr, // block bitmap block index
            BLOCK_SIZE,
            inodemap_block_nr, // inode bitmap block index
            BLOCK_SIZE,
        inode_table_block_nr, // inode table block index
            inode_tablesize,
            2, // first free inode number
        first_free_block, // first free block number
        inode_per_block, // inode per block
        NULL, // root inode
    };

    if(blocks_nr < 8){
        XDEBUG(("block nr %d\n", blocks_nr));
        return -1;
    }

    if(block_in_use >= 32){
        XDEBUG(("block in use %d\n", block_in_use));
        return -1;
    }


    memset(&root_node, 0, sizeof(inode_t));
    root_node.i_mode = 0x41c0; // drwx------
    root_node.i_nlinks = 1;
    root_node.i_mtime = now;
    root_node.i_atime = now;
    root_node.i_ctime = now;
    root_node.i_mode = S_IFDIR | 0x755;
    root_node.i_zone[0] = root_node_block_nr;
    root_node.i_num = root_inode_num; //root node

    root_node.i_ndblock = inode_table_block_nr;

    memcpy(pdisk, &superblock, sizeof(superblock));
    pdisk += superblock.s_superblock_size;

    bitval = setBits(block_in_use);
    XDEBUG(("block in use %d block map  %08x\n",block_in_use, bitval));
    memcpy(pdisk, &bitval, sizeof(unsigned int));
    pdisk += superblock.s_blockmap_size;

    //inode map, first bit is set for root inode
    bitval = 0;
    bitmap_set_nbits(&bitval, 1, 0, 2);
    XDEBUG(("inode map  %08x\n", bitval));
    memcpy(pdisk, &bitval , sizeof(unsigned int));
    pdisk += superblock.s_inodemap_size;
    //inode table, first one is used by root node
    memcpy(pdisk + INODE_STRUCT_SIZE, &root_node, sizeof(inode_t));
    pdisk += superblock.s_inode_table_size;
    XDEBUG(("curr block idx %d, ptr %08x\n", (pdisk - disk_raw) / BLOCK_SIZE, pdisk));

    disk_word_t *pbak = pdisk;
    memset(&d1, 0, sizeof(struct dirent));
    memset(&d2, 0, sizeof(struct dirent));

    d1.d_ino = 1;
    memcpy(&d1.d_name, ".", 2);
    d2.d_ino = 1;
    memcpy(&d2.d_name, "..", 3);

    pdir = (struct dirent*)pdisk;
    memcpy(pdir, &d1, sizeof(struct dirent));
    pdir++;
    memcpy(pdir, &d2, sizeof(struct dirent));

    struct dirent* bak = (struct dirent*)pbak;
    struct dirent* dir = disk_raw + (6 * BLOCK_SIZE);
    for(; dir < (struct dirent* )(pbak + BLOCK_SIZE); dir++ ){
        if(dir->d_ino == 0){
            break;
        }
        printf("Inode %d %s\n", dir->d_ino, dir->d_name);
    }

    disk_word_t curr = 0;
    printf("\nsuper block 0 - 0x%08x\n", curr);
    curr += superblock.s_superblock_size;
    printf("block map 0x%08x - 0x%08x\n",curr, curr+ superblock.s_blockmap_size );
    curr += superblock.s_blockmap_size;
    printf("inode map 0%08x - 0x%08x\n",curr, curr+superblock.s_inodemap_size );
    curr += superblock.s_inodemap_size;
    printf("inode table 0x%08x - 0x%08x\n",curr, curr+superblock.s_inode_table_size );
    curr += superblock.s_inode_table_size;
    printf("data block 0x%08x - 0x%x\n Number of free blocks %d\n",curr, disk_raw + disk_size_words - curr, remaining_blocks );
    
    return 0;
    // return DISK_RAW;
}