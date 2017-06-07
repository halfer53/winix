#include "fs.h"
#include "makefs.h"

int main(){
    makefs();
    struct super_block superblock;
    superblock = {
        0, //magic
        "WINIX_ROOTFS", //name
        64, //blocks in use
        1, //inode in use
        16320, //free blocks
        495, //free inodes
        1024, //block size
        128, //inode size
        1, //root inode number
        1, //block bitmap sector index
        2, //block bitmap sector size
        3, //inode bitmap sector index
        1, //inode bitmap sector size
        4, //inode table sector index
        124, //inode table sector len
        2, //first free inode number
        65, //first free block number
        4,
        NULL
        };
    struct super_block* sb = superblock;
    sb->s_iroot = get_inode(1);
}