//
// Created by bruce on 17/05/20.
//

#include <fs/fs.h>

#define DIRECT_BLOCK_IO 

const char* DEVICE_NAME = "sda";
const char* FS_TYPE = "wfs";

static char* rootfs_disk;
static size_t rootfs_disk_size;

struct device rootfs_dev;
struct superblock root_sb;
//static struct file_system root_fs;
dev_t devid = ROOT_DEV;

struct superblock* get_sb(struct device* id){
    if(id == &rootfs_dev)
        return &root_sb;
    return NULL;
}

int blk_dev_io_read_write(char *buf, off_t off, size_t len, bool write_mode){
    char *ptr;
    int count = len;
    if(off >= rootfs_disk_size)
        return 0;

    if(off + len > rootfs_disk_size){
        count = rootfs_disk_size - off;
        len = count;
    }
//    KDEBUG(("dev write blk %d %d\n", off / BLOCK_SIZE, len));
    ptr = rootfs_disk + off;
    while(count-- > 0){
        if(write_mode){
            *ptr++ = *buf++;
        }else{
            *buf++ = *ptr++;
        }
        
    }
    return len;
}

int blk_dev_io_read(char *buf, off_t off, size_t len){
    return blk_dev_io_read_write(buf, off, len, false);
}

int blk_dev_io_write(char *buf, off_t off, size_t len){
    return blk_dev_io_read_write(buf, off, len, true);
}

void __blk_dev_init(char *disk, size_t size){
    rootfs_disk_size = size;
    rootfs_disk = disk;
    memcpy(&root_sb, rootfs_disk, sizeof(struct superblock));
    arch_superblock(&root_sb);
    ASSERT(root_sb.magic == SUPER_BLOCK_MAGIC);
    // KDEBUG(("sb block in use %d inode table size %d\n", sb->s_block_inuse, sb->s_inode_table_size));
}

int blk_dev_init(){
    __blk_dev_init(DISK_RAW, DISK_SIZE);
    return 0;
}

void arch_superblock(struct superblock* sb){
#ifdef __wramp__
    ARCH_CHAR_SIZE_WRAMP(sb->s_inode_size);
    ARCH_CHAR_SIZE_WRAMP(sb->s_block_size);
    ARCH_CHAR_SIZE_WRAMP(sb->s_superblock_size);
    ARCH_CHAR_SIZE_WRAMP(sb->s_blockmap_size);
    ARCH_CHAR_SIZE_WRAMP(sb->s_inodemap_size);
    ARCH_CHAR_SIZE_WRAMP(sb->s_inode_table_size);
#endif
}

void dearch_superblock(struct superblock* sb){
#ifdef __wramp__
    DEARCH_CHAR_SIZE_WRAMP(sb->s_inode_size);
    DEARCH_CHAR_SIZE_WRAMP(sb->s_block_size);
    DEARCH_CHAR_SIZE_WRAMP(sb->s_superblock_size);
    DEARCH_CHAR_SIZE_WRAMP(sb->s_blockmap_size);
    DEARCH_CHAR_SIZE_WRAMP(sb->s_inodemap_size);
    DEARCH_CHAR_SIZE_WRAMP(sb->s_inode_table_size);
#endif
}

int blk_dev_release(){
    return 0;
}

#ifdef DIRECT_BLOCK_IO

static int init_block(struct block_buffer *buf){
    return 0;
}

static int retrieve_block(struct block_buffer *buf, struct device *dev, block_t bnr){
    off_t off = bnr * BLOCK_SIZE;
    int ret;
    if (off >= rootfs_disk_size){
        ret = -ENOSPC;
        return ret;
    }
    buf->block = rootfs_disk + off;
    return BLOCK_SIZE;
}

static int flush_block(struct block_buffer *buf){
    return BLOCK_SIZE;
}

static int release_block(struct block_buffer *buf){
    return 0;
}

static struct block_operations bops = {init_block, retrieve_block, flush_block, release_block};

#else

static int buffered_init_block(struct block_buffer *buf){
    buf->block = (char*)get_free_page(GFP_HIGH);
    return 0;
}

static int buffered_retrieve_block(struct block_buffer *buf, struct device *dev, block_t bnr){
    off_t off = bnr * BLOCK_SIZE;
    return blk_dev_io_read(buf->block, off, BLOCK_SIZE);
}

static int buffered_flush_block(struct block_buffer *buf){
    off_t off = buf->b_blocknr * BLOCK_SIZE;
    return blk_dev_io_write(buf->block, off, BLOCK_SIZE);
}

static int buffered_release_block(struct block_buffer *buf){
    release_pages((ptr_t *)buf->block, PAGE_LEN);
    return 0;
}
static struct block_operations bops = {buffered_init_block, buffered_retrieve_block, buffered_flush_block, buffered_release_block};


#endif

int root_fs_read_write(struct filp *filp, char *data, size_t count, off_t offset, bool write_mode){
    int r, ret = 0, result;
    unsigned int len;
    off_t off;
    unsigned int curr_fp_index;
    block_t bnr;
    inode_t *ino = NULL;
    struct block_buffer* buffer = NULL;
    struct zone_iterator iter;

    curr_fp_index = offset / BLOCK_SIZE;
    off = offset % BLOCK_SIZE;
    ino = filp->filp_ino;

    if (!write_mode){
        off_t remaining = ino->i_size - offset;
        count = count < remaining ? count : remaining;
    }

    iter_zone_init(&iter, ino, curr_fp_index);
    while(count > 0){
        if(!iter_zone_has_next(&iter)){
            if(!write_mode)
                break;
            
            result = iter_zone_alloc(&iter);
            if(result < 0){
                if(ret == 0)
                    ret = result;
                break;
            }
        }

        bnr = iter_zone_get_next(&iter);
        len = ((BLOCK_SIZE - off) > count) ? count : BLOCK_SIZE - off;
        r = 0;

        if(filp->filp_flags & O_DIRECT){
            off += bnr * BLOCK_SIZE;
            r = blk_dev_io_read_write(data, off, len, write_mode);
            data += r;
        } else{
            char *p;
            size_t len2 = len;
            buffer = get_block_buffer(bnr, filp->filp_dev);
            p = &buffer->block[off];
            while(len2-- > 0){
                if(write_mode){
                    *p++ = *data++;
                }else{
                    *data++ = *p++;
                }
            }
            r += (int)len;
            if(write_mode)
                buffer->b_dirt = true;
            put_block_buffer(buffer);
        }
        // KDEBUG(("file write for block %d, off %d len %d, size %d\n", curr_fp_index, off, r, filp->filp_ino->i_size + r));
        /* Read or write 'chunk' bytes. */
        if (r == 0)
            break;
        count -= len;
        ret += r;
        filp->filp_pos += r;
        if(write_mode)
            filp->filp_ino->i_size += r;
        off = 0;
    }
    // KDEBUG(("Rootfs %d write count %d, offset %d ret %d data %s\n",filp->filp_ino->i_num, count, offset, ret, get_buffer_data(data, count)));
    iter_zone_close(&iter);
    return ret;
}

int root_fs_read (struct filp *filp, char *data, size_t count, off_t offset){
    return root_fs_read_write(filp, data, count, offset, false);
}

int root_fs_write (struct filp *filp, char *data, size_t count, off_t offset){
    return root_fs_read_write(filp, data, count, offset, true);
}

int root_fs_open (struct device* dev, struct filp *file){
    return 0;
}

int root_fs_close (struct device* dev, struct filp *filp){
    filp->filp_count -= 1;
    if(filp->filp_count == 0){
        put_inode(filp->filp_ino, true);
        // KDEBUG(("closing count %d link %d\n", filp->filp_ino->i_count, filp->filp_ino->i_nlinks));
        if(filp->filp_ino->i_count == 0 && filp->filp_ino->i_nlinks == 0){
            release_inode(filp->filp_ino);
        }
    }
    return OK;
}

int root_fs_ioctl(struct filp* file, int request_type, ptr_t* arg){
    return OK;
}

static struct device_operations dops = {blk_dev_init, blk_dev_io_read, blk_dev_io_write, blk_dev_release};
static struct filp_operations ops = {root_fs_open, root_fs_read, root_fs_write, root_fs_close, root_fs_ioctl};

void init_root_fs(){
    rootfs_dev.bops = &bops;
    register_device(&rootfs_dev, DEVICE_NAME, devid, S_IFREG, &dops, &ops);
}

