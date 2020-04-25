#include "fs.h"
#include <winix/list.h>
#include <string.h>

struct device _root_dev;
struct device devices;
struct superblock root_sb;
static struct file_system root_fs;
struct filp_operations ops;
dev_t devid = 0;

char* DEVICE_NAME = "sda";
char* FS_TYPE = "wfs";

disk_word_t* _disk;
size_t _disk_size;


struct superblock* get_sb(struct device* id){
    if(id == root_fs.device)
        return root_fs.sb;
    return NULL;
}

void sync_sb(struct superblock* sb){

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

int blk_dev_io_read(disk_word_t *buf, off_t off, size_t len){
    disk_word_t *ptr;
    int count = len;
    if(off + len > _disk_size)
        return 0;
    ptr = _disk + off;
    while(count-- > 0){
        *buf++ = *ptr++;
    }
    return len;
}

int blk_dev_io_write(disk_word_t *buf, off_t off, size_t len){
    disk_word_t *ptr;
    int count = len;
    if(off + len > _disk_size)
        return 0;
    ptr = _disk + off;
    while(count-- > 0){
        *ptr++ = *buf++;
    }
    return len;
}

int blk_dev_init(){
    _disk_size = DISK_SIZE;
    _disk = DISK_RAW;

    memcpy(root_fs.sb, _disk, sizeof(struct superblock));
    return 0;
}

int blk_dev_destroy(){
    return 0;
}

int rw_chunk(struct filp* filp, int off, int len, int curr_fp_index, char *buf, int flag) {
    int j, ret = 0;
    block_buffer_t *buffer = get_block_buffer(curr_fp_index, filp->filp_dev);

    if (flag == READING) {
        off_t ino_size = filp->filp_ino->i_size;
        for (j = off; j< off + len && j < BLOCK_SIZE && j < ino_size; j++) {
            *buf++ = buffer->block[j];
            ret++;
        }
        XDEBUG(("file read for block %d, off %d len %d\n", curr_fp_index, off, len));
        put_block_buffer(buffer);
    }
    else {
        for (j = off; j< off + len && j < BLOCK_SIZE; j++) {
            buffer->block[j] = *buf++;
            ret++;
        }
        XDEBUG(("file write for block %d, off %d len %d\n", curr_fp_index, off, len));
        put_block_buffer_dirt(buffer);
    }
    return ret;
}

int rw_file(filp_t *filp, char *buf, size_t count, off_t offset, int flag){

}

int root_fs_read (struct filp *filp, char *data, size_t count, off_t offset){
    int ret, r, len, j;
    off_t off, ino_size;
    int curr_fp_index, fp_limit;
    block_t bnr;
    inode_t *ino = NULL;
    struct block_buffer* buffer;

    curr_fp_index = offset / BLOCK_SIZE;
    off = offset % BLOCK_SIZE;
    fp_limit = (filp->filp_pos + count ) / BLOCK_SIZE;
    ino = filp->filp_ino;
    ino_size = ino->i_size;

    for( ; curr_fp_index <= fp_limit; curr_fp_index++){
        len = ((BLOCK_SIZE - off) > count) ? count : BLOCK_SIZE - off;
        bnr = ino->i_zone[curr_fp_index];
        if(bnr == 0){
            break;
        }

        r = 0;
        buffer = get_block_buffer(bnr, filp->filp_dev);
        for (j = off; j< off + len && j < BLOCK_SIZE && j < ino_size; j++) {
            *data++ = buffer->block[j];
            r++;
        }
        XDEBUG(("file read for block %d, off %d len %d\n", curr_fp_index, off, r));
        put_block_buffer(buffer);
        if (r == 0)
            break;

        count -= len;
        ret += r;
        filp->filp_pos += r;
        off = 0;
    }
    return ret;
}

int root_fs_write (struct filp *filp, const char *data, size_t count, off_t offset){
    int r, len, j, ret;
    off_t off;
    int curr_fp_index, fp_limit;
    block_t bnr;
    inode_t *ino = NULL;
    struct block_buffer* buffer;

    curr_fp_index = offset / BLOCK_SIZE;
    off = offset % BLOCK_SIZE;
    fp_limit = (filp->filp_pos + count ) / BLOCK_SIZE;
    ino = filp->filp_ino;

    for( ; curr_fp_index <= fp_limit; curr_fp_index++){
        len = ((BLOCK_SIZE - off) > count) ? count : BLOCK_SIZE - off;
        bnr = ino->i_zone[curr_fp_index];
        if(bnr == 0){
            if(curr_fp_index < NR_TZONES && (bnr = alloc_block(ino, ino->i_dev)) > 0){
                ino->i_zone[curr_fp_index] = bnr;
            }else{
                return ENOSPC;
            }
        }
        buffer = get_block_buffer(bnr, filp->filp_dev);

        r = 0;
        for (j = off; j< off + len && j < BLOCK_SIZE && j < ino->i_size; j++) {
            buffer->block[j] = *data++;
            r++;
        }
        XDEBUG(("file write for block %d, off %d len %d\n", curr_fp_index, off, r));
        put_block_buffer_dirt(buffer);
        /* Read or write 'chunk' bytes. */
        if (r == 0)
            break;

        count -= len;
        ret += r;
        filp->filp_pos += r;
        filp->filp_ino->i_size += r;
        off = 0;
    }
    return r;
}

int root_fs_open (struct inode* ino, struct filp *file){
    return 0;
}

int root_fs_close (struct inode* ino, struct filp *file){
    return 0;
}

void init_dev(){
    INIT_LIST_HEAD(&devices.list);
    list_add(&_root_dev.list, &devices.list);
}

struct device* get_dev(dev_t dev){
    struct device* ret;
    list_for_each_entry(struct device, ret, &devices.list, list){
        if(ret->dev_id == dev){
            return ret;
        }
    }
    return NULL;
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
    dev->dev_register = blk_dev_init;
    dev->dev_destroy = blk_dev_destroy;
    dev->dev_read = blk_dev_io_read;
    dev->dev_write = blk_dev_io_write;

    ops.open = root_fs_open;
    ops.write = root_fs_write;
    ops.read = root_fs_read;
    ops.close = root_fs_close;
    dev->fops = &ops;

    dev->dev_register();

    ino = get_inode(ROOT_INODE_NUM, dev);
    root_fs.sb->s_iroot = ino;
    current_proc->fp_workdir = current_proc->fp_rootdir = ino;
}


