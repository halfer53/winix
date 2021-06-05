#include "fs.h"
#include <stdio.h>
#include <string.h>

char rootfs_name[] = "WINIX_ROOTFS";

unsigned int setBits(unsigned int n)
{
    unsigned int ret = 0;
    bitmap_set_nbits(&ret, 1,0,n);
    return ret;
}

int makefs( char* disk_raw, size_t disk_size)
{
    char *pdisk = disk_raw;
    struct winix_dirent* pdir;
    const time_t now = start_unix_time;
    const int root_inode_num = 1;
    inode_t root_node;
    unsigned int blocks_nr = disk_size / BLOCK_SIZE;
    block_t sb_block_nr = 0;
    block_t blockmap_block_nr = 1;
    block_t inodemap_block_nr = 2;
    block_t inode_table_block_nr = 3;
    unsigned int inode_tablesize = (int)(blocks_nr * 0.02) * BLOCK_SIZE;
    block_t root_node_block_nr = inode_table_block_nr + (inode_tablesize / BLOCK_SIZE);
    block_t first_free_block = root_node_block_nr + 1;
    block_t block_in_use = root_node_block_nr + 1;
    block_t remaining_blocks = blocks_nr - block_in_use;
    unsigned int remaining_words = remaining_blocks * BLOCK_SIZE;
    int inode_per_block = BLOCK_SIZE / INODE_DISK_SIZE;
    unsigned int free_inodes = inode_tablesize / INODE_DISK_SIZE - 1;
    unsigned int bitval = 0;
    struct superblock s2;

    struct superblock superblock = {
            SUPER_BLOCK_MAGIC, // magic
        block_in_use, // blocks in use
        1, // inode in use
            remaining_blocks, // free blocks
        free_inodes, // free inodes
        BLOCK_SIZE, // block size
            INODE_DISK_SIZE, // inode size
            root_inode_num, // root inode number

        sb_block_nr,
        BLOCK_SIZE,
            blockmap_block_nr, // block bitmap block index
            BLOCK_SIZE,
            inodemap_block_nr, // inode bitmap block index
            BLOCK_SIZE,
        inode_table_block_nr, // inode table block index
            inode_tablesize,
        inode_per_block, // inode per block
        NULL, // root inode
    };
    char32_strlcpy(superblock.s_name, rootfs_name, SUPERBLOCK_NAME_LEN);
    // printf("block nr %d %d %d\n", blocks_nr, block_in_use, remaining_blocks);
    if(blocks_nr < 8){
        KDEBUG(("block nr %d\n", blocks_nr));
        return -1;
    }

    if(block_in_use >= 32){
        KDEBUG(("block in use %d\n", block_in_use));
        return -1;
    }


    memset(&root_node, 0, sizeof(inode_t));
    root_node.i_mode = S_IFDIR | 0x755;
    root_node.i_nlinks = 1;
    root_node.i_mtime = now;
    root_node.i_atime = now;
    root_node.i_ctime = now;
    root_node.i_zone[0] = root_node_block_nr;
    root_node.i_num = ROOT_INODE_NUM; //root node
    root_node.i_ndblock = inode_table_block_nr;
    root_node.i_size = BLOCK_SIZE;

    memcpy(&s2, &superblock, sizeof(struct superblock));
    dearch_superblock(&s2);
    memcpy(pdisk, &s2, sizeof(superblock));
    pdisk += superblock.s_superblock_size;

    bitval = setBits(block_in_use);
//    KDEBUG(("block in use %d block map  %08x\n",block_in_use, bitval));
    memcpy(pdisk, &bitval, sizeof(unsigned int));
    pdisk += superblock.s_blockmap_size;

    //inode map, first bit is set for root inode
    bitval = 0;
    bitmap_set_nbits(&bitval, 1, 0, 2);
    memcpy(pdisk, &bitval , sizeof(unsigned int));
    pdisk += superblock.s_inodemap_size;
    //inode table, first one is used by root node
    memcpy(pdisk + INODE_DISK_SIZE, &root_node, INODE_DISK_SIZE);
    pdisk += superblock.s_inode_table_size;

    char *pbak = pdisk;
    pdir = (struct winix_dirent*)pdisk;
    fill_dirent(&root_node, pdir, ".");
    pdir++;
    fill_dirent(&root_node, pdir, "..");

    return 0;
    // return DISK_RAW;
}