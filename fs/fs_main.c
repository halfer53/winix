#include "fs.h"
#include "makefs.h"
#include "inode.h"
#include <stdio.h>

struct proc pcurrent_proc;
struct proc *current_proc;
struct super_block *sb;
struct super_block superblock = {
        0, // magic
        "WINIX_ROOTFS", // name
        65, // blocks in use
        1, // inode in use
        16319, // free blocks
        495, // free inodes
        1024, // block size
        128, // inode size
        1, // root inode number
        1, // block bitmap block index
        2, // inode bitmap block index
        3, // inode table block index
        2, // first free inode number
        67, // first free block number
        8, // inode per block
        NULL, // root inode
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

//     block_t s_blockmapnr; // block map sector index
//     block_t s_inodemapnr; // inode map sector index
//     block_t s_inode_tablenr; // inode map sector index
//     int s_ninode; // first free inode number
//     int s_nblock; // first free block number

//     int s_inode_per_block;
//     inode_t *s_iroot;
// };


void init_fs() {

    printf("\nNum of blocks in use %d\n", sb->s_block_inuse);
    printf("First free block num: %d\n", sb->s_nblock);
    printf("Block size %d\n inode size %d\n", sb->s_block_size, sb->s_inode_size);
    printf("inode per block %d\n", sb->s_inode_per_block);
}

int main(){
    
    int ret = makefs();
    inode_t *rootinode;
    sb = &superblock;
    int i;

    
    init_buf();
    init_inode();
    init_filp();

    sb->s_iroot = get_inode(1);
    sb->s_iroot->i_nlinks += 1;
    current_proc = &pcurrent_proc;
    current_proc->fp_workdir = current_proc->fp_rootdir = sb->s_iroot;
    init_fs();


    char abc[] = "abcdefghijklmnopqrstuvwxyz";
    char c = 'a';
    int fd = sys_open(current_proc, "/foo.txt",O_CREAT);
    for (i = 0; i < 2048; i++) {
        sys_write(current_proc, fd, &c, 1);
        c++;
        if (c == 'z')
            c = 'a';
    }
    sys_write(current_proc,fd, "a", 2);
    sys_close(current_proc, fd);

    char buf[1024];
    fd = sys_open(current_proc, "/foo.txt",O_RDONLY);
    sys_read(current_proc, fd,buf,2049);
    sys_close(current_proc, fd);

    printf("\nread foo.txt\n");
    printf("Got \"%s\" from foo.txt\n",buf);

}