#include "fs.h"
#include "makefs.h"
#include <stdio.h>

int main(){
    makefs();
    struct super_block superblock;
    superblock = (struct super_block){
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
    init_buf();
    struct super_block* sb = superblock;
    sb->s_iroot = get_inode(1);

    proc_t pcurrent_proc;
    pcurrent_proc.fp_roodir = pcurrent_proc.fp_workdir = sb->s_iroot;
    proc_t *current_proc = &tcurrent_proc;
    
    printf("%d"sb->s_iroot->i_zone[0]);
}