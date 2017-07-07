#include "fs.h"
#include "makefs.h"
#include "inode.h"
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
        67, //first free block number
        8, //inode per block
        NULL
    };

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

int main(){
    int ret = makefs();
    inode_t *rootinode;
    sb = &superblock;
    init_buf();
    init_inode();
    init_filp();
    sb->s_iroot = get_inode(1);
    sb->s_iroot->i_nlinks += 1;

    current_proc = &pcurrent_proc;
    current_proc->fp_rootdir = sb->s_iroot;
    current_proc->fp_workdir = sb->s_iroot;

    printf("\nNum of blocks in use %d\n",sb->s_block_inuse);
    printf("First free block num: %d\n",sb->s_nblock);
    printf("Block size %d\n inode size %d\n",sb->s_block_size, sb->s_inode_size);
    printf("inode per block %d\n",sb->s_inode_per_block);

    // printf("begin\n");
    // printf("root dir %d \n",current_proc->fp_rootdir->i_num);

    int fd = sys_open("/foo.txt",O_CREAT);
    printf("\nwrite \"bar\" into file foo.txt\n");
    sys_write(fd,"bar",4);
    sys_close(fd);

    char buf[4];
    printf("open foo.txt\n");
    fd = sys_open("/foo.txt",O_RDONLY);
    printf("read foo.txt\n");
    sys_read(fd,buf,4);
    sys_close(fd);

    printf("Got \"%s\" from foo.txt\n",buf);

}