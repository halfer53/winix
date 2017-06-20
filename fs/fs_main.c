#include "fs.h"
#include "makefs.h"
#include <stdio.h>

proc_t pcurrent_proc;
proc_t *current_proc;
struct super_block *sb;
struct super_block superblock = {
        0, //magic
        "WINIX_ROOTFS", //name
        65, //blocks in use
        1, //inode in use
        16319, //free blocks
        495, //free inodes
        1024, //block size
        128, //inode size
        1, //root inode number
        1, //block bitmap block index
        2, //inode bitmap block index
        3, //inode table block index
        2, //first free inode number
        66, //first free block number
        8, //inode per block
        NULL
    };

int main(){
    int ret = makefs();
    printf("Fs done\n");
    
    // struct super_block {
    //     int magic;
    //     char s_name[32];
    //     int s_block_inuse;
    //     int s_inode_inuse;
    //     int s_free_blocks;
    //     int s_free_inodes;
    //     int s_block_size;
    //     size_t s_inode_size;
    //     int s_rootnr;    
        
    //     block_t s_blockmapnr; //block map sector index
    //     block_t s_inodemapnr; //inode map sector index
    //     block_t s_inode_tablenr; //inode map sector index
    //     int s_ninode; //first free inode number
    //     int s_nblock; //first free block number

    //     int s_inode_per_block;
    //     inode_t *s_iroot;
    // };
    
    sb = &superblock;
    init_buf();
    pcurrent_proc.fp_rootdir = pcurrent_proc.fp_workdir = sb->s_iroot;
    current_proc = &pcurrent_proc;

    sb->s_iroot = get_inode(1);

    printf("%d",sb->s_iroot->i_zone[0]);
}