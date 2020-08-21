//
// Created by bruce on 17/05/20.
//

#include "fs.h"

char* DEVICE_NAME = "sda";
char* FS_TYPE = "wfs";

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

int blk_dev_io_read(char *buf, off_t off, size_t len){
    char *ptr;
    int count = len;
    if(off >= rootfs_disk_size)
        return 0;

    if(off + len > rootfs_disk_size){
        count = rootfs_disk_size - off;
        len = count;
    }

//    KDEBUG(("dev read blk %d %d\n", off / BLOCK_SIZE, len));
    ptr = rootfs_disk + off;
    while(count-- > 0){
        *buf++ = *ptr++;
    }
    return len;
}

int blk_dev_io_write(char *buf, off_t off, size_t len){
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
        *ptr++ = *buf++;
    }
    return len;
}

int blk_dev_init(){
    struct superblock* sb;
    rootfs_disk_size = DISK_SIZE;
    rootfs_disk = DISK_RAW;
#ifdef __wramp__
    ASSERT(DISK_RAW[0] == SUPER_BLOCK_MAGIC);
#endif
    memcpy(&root_sb, rootfs_disk, sizeof(struct superblock));
    arch_superblock(&root_sb);
    ASSERT(root_sb.magic == SUPER_BLOCK_MAGIC);
    // KDEBUG(("sb block in use %d inode table size %d\n", sb->s_block_inuse, sb->s_inode_table_size));
    return 0;
}

void arch_superblock(struct superblock* sb){
#ifdef __wramp__
    sb->s_inode_size /= 4;
    sb->s_block_size /= 4;
    sb->s_superblock_size /= 4;
    sb->s_blockmap_size /= 4;
    sb->s_inodemap_size /= 4;
    sb->s_inode_table_size /= 4;
#endif
}

void dearch_superblock(struct superblock* sb){
#ifdef __wramp__
    sb->s_inode_size *= 4;
    sb->s_block_size *= 4;
    sb->s_superblock_size *= 4;
    sb->s_blockmap_size *= 4;
    sb->s_inodemap_size *= 4;
    sb->s_inode_table_size *= 4;
#endif
}

int blk_dev_release(){
    return 0;
}

int root_fs_read (struct filp *filp, char *data, size_t count, off_t offset){
    int ret = 0, r, j;
    unsigned int len;
    off_t off, end_in_block;
    unsigned int curr_fp_index, fp_limit;
    block_t bnr;
    inode_t *ino = NULL;
    struct block_buffer* buffer = NULL;

    curr_fp_index = offset / BLOCK_SIZE;
    off = offset % BLOCK_SIZE;
    fp_limit = (filp->filp_pos + count ) / BLOCK_SIZE;
    ino = filp->filp_ino;

    for( ; curr_fp_index <= fp_limit; curr_fp_index++){
        bnr = ino->i_zone[curr_fp_index];
        if(bnr == 0)
            continue;

        len = ((BLOCK_SIZE - off) > count) ? count : BLOCK_SIZE - off;
        len = (off + len) < ino->i_size ? len : ino->i_size - off;
        r = 0;
        if(filp->filp_flags & O_DIRECT){
            off += bnr * BLOCK_SIZE;
            r = filp->filp_dev->dops->dev_read(data, off, len);
            data += r;
        }else{
            char *p;
            size_t len2 = len;
            buffer = get_block_buffer(bnr, filp->filp_dev);
            p = &buffer->block[off];
            while(len2-- > 0){
                *data++ = *p++;
            }
            r += (int)len;
            put_block_buffer(buffer);
        }

        count -= len;
        ret += r;
        filp->filp_pos += r;
        // KDEBUG(("file read for block %d, off %d len %d remaining %d\n", bnr, off, r, count));
        if (r == 0)
            break;
        off = 0;
    }
    return ret;
}

int root_fs_write (struct filp *filp, char *data, size_t count, off_t offset){
    int r, j, ret = 0;
    unsigned int len;
    off_t off;
    unsigned int curr_fp_index, fp_limit;
    block_t bnr;
    inode_t *ino = NULL;
    struct block_buffer* buffer = NULL;

    curr_fp_index = offset / BLOCK_SIZE;
    off = offset % BLOCK_SIZE;
    fp_limit = (filp->filp_pos + count ) / BLOCK_SIZE;
    ino = filp->filp_ino;

    for( ; curr_fp_index <= fp_limit; curr_fp_index++){
        bnr = ino->i_zone[curr_fp_index];
        if(bnr == 0){
            if((bnr = alloc_block(ino, ino->i_dev)) > 0){
                ino->i_zone[curr_fp_index] = bnr;
            }else{
                return ENOSPC;
            }
        }

        len = ((BLOCK_SIZE - off) > count) ? count : BLOCK_SIZE - off;
        r = 0;
        if(filp->filp_flags & O_DIRECT){
            off += bnr * BLOCK_SIZE;
            r = filp->filp_dev->dops->dev_write(data, off, len);
            data += r;
        } else{
            char *p;
            size_t len2 = len;
            buffer = get_block_buffer(bnr, filp->filp_dev);
            p = &buffer->block[off];
            while(len2-- > 0){
                *p++ = *data++;
            }
            r += (int)len;
            put_block_buffer_dirt(buffer);
        }
        // KDEBUG(("file write for block %d, off %d len %d, size %d\n", curr_fp_index, off, r, filp->filp_ino->i_size + r));
        /* Read or write 'chunk' bytes. */
        if (r == 0)
            break;

        count -= len;
        ret += r;
        filp->filp_pos += r;
        filp->filp_ino->i_size += r;
        off = 0;
    }
    return ret;
}

int root_fs_open (struct device* dev, struct filp *file){
    return 0;
}

int root_fs_close (struct device* dev, struct filp *filp){
    filp->filp_count -= 1;
    if(filp->filp_count == 0){
        put_inode(filp->filp_ino, true);
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
    register_device(&rootfs_dev, DEVICE_NAME, devid, S_IFREG, &dops, &ops);
}

