#include "fs.h"
#include <winix/list.h>
#include <string.h>

struct device _root_dev;
struct device* devices;
struct superblock root_sb;
static struct file_system root_fs;
dev_t devid = 0;

char* DEVICE_NAME = "sda";
char* FS_TYPE = "wfs";

disk_word_t* _disk;
size_t _disk_size;

int dev_io_read(disk_word_t *buf, block_t blocknr){
    disk_word_t *ptr = _disk + blocknr * BLOCK_SIZE;
    int i;
    XDEBUG(("dev read block %d\n", blocknr));
    for(i = 0; i < BLOCK_SIZE; i++){
        buf[i] = ptr[i];
    }
    return BLOCK_SIZE;
}

int dev_io_write(disk_word_t *buf, block_t blocknr){
    disk_word_t *ptr = _disk + blocknr * BLOCK_SIZE;
    int i;
    XDEBUG(("dev write block %d\n",  blocknr));
    for(i = 0; i < BLOCK_SIZE; i++){
        ptr[i] = buf[i];
    }
    return BLOCK_SIZE;
}

struct superblock* get_sb(dev_t id){
    if(id == root_fs.device->dev_id)
        return root_fs.sb;
    return NULL;
}


void report_sb(struct superblock* sb){
    disk_word_t curr = 0;
    printf("\nsuper block 0 - 0x%08x\n", curr);
    curr += sb->s_superblock_size;
    printf("block map 0x%08x - 0x%08x\n",curr, curr+ sb->s_blockmap_size );
    curr += sb->s_blockmap_size;
    printf("inode map 0%08x - 0x%08x\n",curr, curr+sb->s_inodemap_size );
    curr += sb->s_inodemap_size;
    printf("inode table 0x%08x - 0x%08x\n",curr, curr+sb->s_inode_table_size );
    curr += sb->s_inode_table_size;
    printf("data block 0x%08x - 0x%p\n Number of free blocks %d\n",curr, _disk + _disk_size - curr, sb->s_free_blocks );

    printf("\nNum of blocks in use %d\n", sb->s_block_inuse);
    printf("First free block num: %d\n", sb->s_nblock);
    printf("Block size %d\n inode size %d\n", sb->s_block_size, sb->s_inode_size);
    printf("inode per block %d\n", sb->s_inode_per_block);
}

void fill_sb(struct superblock* sb){

}

int root_device_init(){
    _disk_size = DISK_SIZE;
    _disk = DISK_RAW;

    memcpy(root_fs.sb, _disk, sizeof(struct superblock));
    return 0;
}

int root_device_destroy(){
    return 0;
}

int root_fs_read (struct filp *file, char *data, size_t len, off_t * offset){
    return 0;
}

int root_fs_write (struct filp *file, const char *data, size_t len, off_t *offset){
    return 0;
}

int root_fs_open (struct inode* ino, struct filp *file){
    return 0;
}

int root_fs_close (struct inode* ino, struct filp *file){
    return 0;
}

void init_dev(){
    devices = &_root_dev;
    INIT_LIST_HEAD(&_root_dev.list);
}

void init_root_fs(){
    struct superblock test;
    struct inode* ino;
    struct device *dev = &_root_dev;
    devid = MAKEDEV(1, 2);
    INIT_LIST_HEAD(&root_fs.list);
    root_fs.device = dev;
    root_fs.sb = &root_sb;
    root_fs.type = FS_TYPE;
    dev->dev_id = devid;
    dev->init_name = DEVICE_NAME;
    dev->dev_register = root_device_init;
    dev->dev_destroy = root_device_destroy;
    dev->ops.open = root_fs_open;
    dev->ops.write = root_fs_write;
    dev->ops.read = root_fs_read;
    dev->ops.close = root_fs_close;


    dev->dev_register();

    ino = get_inode(1, devid);
    root_fs.sb->s_iroot = ino;
    root_fs.sb->s_iroot->i_nlinks += 1;
    current_proc->fp_workdir = current_proc->fp_rootdir = ino;
}


