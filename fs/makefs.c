#include <fs/fs.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

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
    unsigned int inode_tablesize = (int)(blocks_nr * 0.03) * BLOCK_SIZE;
    block_t root_node_block_nr = inode_table_block_nr + (inode_tablesize / BLOCK_SIZE);
    block_t block_in_use = root_node_block_nr + 1;
    block_t remaining_blocks = blocks_nr - block_in_use;
    unsigned int free_inodes = inode_tablesize / INODE_DISK_SIZE - 1;
    unsigned int bitval = 0;
    struct superblock s2;

    struct superblock superblock = {
        .magic = SUPER_BLOCK_MAGIC, // magic
        .s_block_inuse = block_in_use, // blocks in use
        .s_inode_inuse = 1, // inode in use
        .s_free_blocks = remaining_blocks, // free blocks
        .s_free_inodes = free_inodes, // free inodes
        .s_block_size = BLOCK_SIZE, // block size
        .s_inode_size = INODE_DISK_SIZE, // inode size
        .s_rootnr = root_inode_num, // root inode number

        .s_superblock_nr = sb_block_nr,
        .s_superblock_size = BLOCK_SIZE,
        .s_blockmapnr = blockmap_block_nr, // block bitmap block index
        .s_blockmap_size = BLOCK_SIZE,
        .s_inodemapnr = inodemap_block_nr, // inode bitmap block index
        .s_inodemap_size = BLOCK_SIZE,
        .s_inode_tablenr = inode_table_block_nr, // inode table block index
        .s_inode_table_size = inode_tablesize,
        .s_char_bit = CHAR_BIT,
    };
    char32_strlcpy(superblock.s_name, rootfs_name, SUPERBLOCK_NAME_LEN);
    // printf("block nr %d %d %d inode table size %ld\n", blocks_nr, block_in_use, remaining_blocks, inode_tablesize / BLOCK_SIZE);
    assert(BLOCK_SIZE > sizeof(struct superblock));
    assert(BLOCK_SIZE_DWORD * 32 >= blocks_nr);
    assert(BLOCK_SIZE_DWORD * 32 >= free_inodes);


    memset(&root_node, 0, sizeof(inode_t));
    root_node.i_mode = S_IFDIR | 0755;
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

    // kdebug("block in use %d\n", block_in_use);
    bitmap_set_nbits((unsigned int *)pdisk, BLOCK_SIZE_DWORD, 0, block_in_use);
    pdisk += superblock.s_blockmap_size;

    //inode map, first bit is set for root inode
    bitval = 0;
    bitmap_set_nbits(&bitval, 1, 0, 2);
    memcpy(pdisk, &bitval , sizeof(unsigned int));
    pdisk += superblock.s_inodemap_size;
    //inode table, first one is used by root node
    memcpy(pdisk + INODE_DISK_SIZE, &root_node, INODE_DISK_SIZE);
    pdisk += superblock.s_inode_table_size;

    pdir = (struct winix_dirent*)pdisk;
    fill_dirent(&root_node, pdir, ".");
    pdir++;
    fill_dirent(&root_node, pdir, "..");

    return 0;
    // return DISK_RAW;
}