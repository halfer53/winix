#include "fs.h"
#include "makefs.h"

int main(){
    makefs();
    struct super_block superblock;
    superblock = {
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
        8,
        NULL
        };
    struct super_block* sb = superblock;
    sb->s_iroot = get_inode(1);
}