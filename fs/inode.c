#include <fs/fs.h>
#include <kernel/clock.h>

int fill_dirent(inode_t* ino, struct winix_dirent* curr, const char* string);

#define INODE_ARC_MAP_LEN   (8)
inode_t inode_table[NR_INODES];

struct inode* get_free_inode_slot(){
    inode_t* rep;
    int i;
    for(i = 0; i < NR_INODES; i++ ){
        rep = &inode_table[i];
        if(rep->i_num == 0){
            memset(rep, 0, sizeof(struct inode));
            return rep;
        }
    }
    return NULL;
}

bool has_file_access(struct proc* who, struct inode* ino, mode_t mode){
    if(who->uid == 0)
        return true;
    if(who->uid == ino->i_uid && (mode << 8) & ino->i_mode){
        return true;
    }
    if(who->gid == ino->i_gid && (mode << 4) & ino->i_mode){
        return true;
    }
    if(mode & ino->i_mode){
        return true;
    }
    return false;

}

bool is_valid_inode_num(int num, struct device* id){
    struct superblock* sb = get_sb(id);
    unsigned int inodes_nr;

    inodes_nr = sb->s_inode_per_block * (sb->s_inode_table_size / BLOCK_SIZE);
//    KDEBUG(("is valid inode num: %d, inode per block %d, inodes_nr %d\n", num, sb->s_inode_per_block, inodes_nr));
    return 1 <= num && num <= inodes_nr;
}

bool is_valid_block_num(block_t bnr, struct device* id){
    struct superblock* sb = get_sb(id);
    unsigned int total_block = sb->s_block_inuse + sb->s_free_blocks;
    return bnr < total_block;
}

bool is_inode_in_use(int num, struct device* id){
    bool ret;
    struct superblock* sb = get_sb(id);
    struct block_buffer *buf;
    // KDEBUG(("is inode in use %d, inode map nr %d\n", num, sb->s_inodemapnr));
    if(!is_valid_inode_num(num, id)){
        return false;
    }
    buf = get_block_buffer(sb->s_inodemapnr, id);
    // KDEBUG(("inode map %08x for inode %d\n", *map_ptr, num));
    ret = is_bit_on((unsigned int*)buf->block, (int)TO_DWORD_SIZE(sb->s_inodemap_size), num);
    put_block_buffer(buf);
    return ret;
}

int alloc_block(inode_t *ino, struct device* id){
    struct superblock* sb = get_sb(id);
    struct block_buffer *bmap = get_block_buffer(sb->s_blockmapnr, id);
    block_t bmap_end = sb->s_blockmapnr + (sb->s_blockmap_size / BLOCK_SIZE);
    block_t bnr = bmap->b_blocknr;
    int free_bit = -1;

    while(bmap->b_blocknr < bmap_end){
        free_bit = bitmap_search_from((unsigned int*) bmap->block, BLOCK_SIZE_DWORD, 0, 1);
        if(free_bit > 0){
            if (!is_valid_block_num(free_bit, id)){
                return -ENOSPC;
            }
            bitmap_set_bit((unsigned int*)bmap->block, BLOCK_SIZE_DWORD, free_bit);
            sb->s_block_inuse += 1;
            sb->s_free_blocks -= 1;
            put_block_buffer_dirt(bmap);
            // KDEBUG(("alloc_block %d for inode %d\n", free_bit, ino->i_num));
            return free_bit;
        }
        put_block_buffer(bmap);
        bnr++;
        bmap = get_block_buffer(bnr, id);
    }
    kwarn("no free block id found for dev %d", id->dev_id);
    put_block_buffer(bmap);
    return -ENOSPC;
}

int release_block(block_t bnr, struct device* id){
    struct superblock* sb = get_sb(id);
    block_t bmap_nr = sb->s_blockmapnr + (bnr / BLOCK_SIZE);
    struct block_buffer *bmap, *block;
    if(!is_valid_block_num(bnr, id)){
        kwarn("Invalid block id %d\n", bnr);
        return -EINVAL;
    }
    block = get_block_buffer(bnr, id);
    memset(block->block, 0, BLOCK_SIZE);
    put_block_buffer_dirt(block);

    bmap = get_block_buffer(bmap_nr, id);

    bitmap_clear_bit((unsigned int*)bmap->block, BLOCK_SIZE_DWORD, bnr);
    sb->s_block_inuse -= 1;
    sb->s_free_blocks += 1;
    return put_block_buffer(bmap);
}

blkcnt_t get_inode_blocks(struct inode* ino){
    blkcnt_t ret = 0;
    struct zone_iterator iter;
    iter_zone_init(&iter, ino, 0);
    while(iter_zone_has_next(&iter)){
        ret++;
        (void)iter_zone_get_next(&iter);
    }
    iter_zone_close(&iter);
    return ret;
}

size_t get_inode_total_size_word(struct inode* ino){
    blkcnt_t bnum = get_inode_blocks(ino);
    return BLOCK_SIZE * bnum;
}

int init_inode_non_disk(struct inode* ino, ino_t num, struct device* dev, struct superblock* sb){
    block_t bnr;
    ino->i_dev = dev;
    ino->i_sb = sb;
    ino->i_num = num;
    // klog("init inode %d dev %u\n", num, dev->dev_id);
    if(sb){
        bnr = ((num * sb->s_inode_size) / BLOCK_SIZE) + sb->s_inode_tablenr;
        if(bnr * BLOCK_SIZE >= sb->s_inode_tablenr * BLOCK_SIZE + sb->s_inode_table_size){
            kwarn("ino %d exceeding\n", num);
            return -EINVAL;
        }
        ino->i_ndblock = bnr;
    }
    return OK;
}

void arch_inode(struct inode* ino){
    
#ifdef __wramp__
    ARCH_CHAR_SIZE(ino->i_size);
    // KDEBUG(("arch %d \n", ino->i_num));
#endif
    
}

void dearch_inode(struct inode* ino){

#ifdef __wramp__
    DEARCH_CHAR_SIZE(ino->i_size);
    // KDEBUG(("dearch %d \n", ino->i_num));
#endif
}

int read_inode(int num, struct inode** ret_ino, struct device* id){
    struct superblock* sb = get_sb(id);
    block_t inode_block_nr = (num * sb->s_inode_size) / BLOCK_SIZE;
    unsigned int offset = (num * sb->s_inode_size) % BLOCK_SIZE;
    block_t blocknr = sb->s_inode_tablenr + inode_block_nr;
    struct block_buffer *buffer;
    struct inode* inode = get_free_inode_slot();

    if(!inode)
        return -ENOSPC;
    if(!is_inode_in_use(num, id))
        return -EINVAL;
    if(num == 0)
        return -EINVAL;

    buffer = get_block_buffer(blocknr, id);
    memcpy(inode, &buffer->block[offset], INODE_DISK_SIZE);
    inode->i_count += 1;
    init_inode_non_disk(inode, num, id, sb);
    arch_inode(inode);

    put_block_buffer(buffer);
    *ret_ino = inode;
    // KDEBUG(("read inode %d blk %d off %d zone %d\n", num, blocknr, offset, inode->i_zone[0]));
    return OK;
}

inode_t* get_inode(int num, struct device* id){
    inode_t* rep = NULL;
    int i, ret;
    for(i = 0; i < NR_INODES; i++ ){
        rep = &inode_table[i];
        if(rep->i_num == num){
//            KDEBUG(("found ino %d in cache\n", num));
            rep->i_count += 1;
            return rep;
        }
    }
    
    ret = read_inode(num, &rep, id);
    if(ret){
        kwarn("read inode %d return %d\n", num, ret);
        return NULL;
    }

    return rep;
}


int put_inode(inode_t *inode, bool is_dirty){
    struct superblock* sb;
    int inum;
    unsigned int inode_block_offset;
    struct block_buffer *buffer;
    if(!inode)
        return -EINVAL;
    inode->i_count -= 1;
    if(!is_dirty)
        return OK;
    sb = get_sb(inode->i_dev);
    inum = inode->i_num;
    inode_block_offset = (inum * sb->s_inode_size) % BLOCK_SIZE;;
    buffer = get_block_buffer(inode->i_ndblock, inode->i_dev);
    dearch_inode(inode);
    memcpy(buffer->block + inode_block_offset, inode, INODE_DISK_SIZE);
    arch_inode(inode);
    put_block_buffer_dirt(buffer);
    inode->i_flags &= ~INODE_FLAG_DIRTY;
    // KDEBUG(("put inode %d blk %d offset %d\n", inode->i_num, inode->i_ndblock, inode_block_offset));
    return OK;
}


inode_t* alloc_inode(struct device* parentdev, struct device* inodev){
    struct superblock* sb;
    int inum = 0;
    block_t imap_end = 0, bnr = 0;
    struct block_buffer *imap;
    inode_t *inode;
    bool found = false;
    clock_t unix_time = get_unix_time();

    sb = get_sb(parentdev);
    imap = get_imap(parentdev);
    imap_end = imap->b_blocknr + (sb->s_blockmap_size / BLOCK_SIZE);
    while(imap->b_blocknr < imap_end){
        inum = bitmap_search_from((unsigned int*)imap->block, BLOCK_SIZE_DWORD, 0, 1);
        if(inum > 0){
            found = true;
            break;
        }
        put_block_buffer(imap);
        bnr++;
        imap = get_block_buffer(bnr, parentdev);
    }
    if(!found){
        put_block_buffer(imap);
        return NULL;
    }
    bitmap_set_bit((unsigned int*)imap->block, BLOCK_SIZE_DWORD, inum);
    put_block_buffer_dirt(imap);

    inode = get_free_inode_slot();
    if (!inode)
        return NULL;
    
    init_inode_non_disk(inode, inum, inodev, sb);
    inode->i_count += 1;
    inode->i_ctime = get_unix_time();
    // KDEBUG(("alloc ino set bit %d\n", inum));
    inode->i_ctime = unix_time;
    inode->i_mtime = unix_time;
    inode->i_atime = unix_time;

    sb->s_free_inodes -= 1;
    sb->s_inode_inuse += 1;
    return inode;
}

void init_inode_proc_field(struct inode* ino, struct proc* who, mode_t devtype, mode_t mode){
    ino->i_gid = who->gid;
    ino->i_uid = who->uid;
    ino->i_mode = devtype | (mode & ~(who->umask));
}

int truncate_inode(inode_t *inode){
    struct zone_iterator iter;
    iter_zone_init(&iter, inode, 0);

    while(iter_zone_has_next(&iter)){
        zone_t zone = iter_zone_get_next(&iter);
        release_block((block_t)zone, inode->i_dev);
    }
    iter_zone_close(&iter);
    return OK;
}


int _release_inode(inode_t *inode, bool is_indirect_zone){
    struct device* id = inode->i_dev;
    int inum = inode->i_num;
    struct superblock* sb = get_sb(id);
    struct block_buffer *imap;
    block_t zone_id;
    int i = 0;
    if(inode->i_count != 0){
        kwarn("%d is in use before releasing\n", inum);
        return -EINVAL;
    }
    // KDEBUG(("releasing inode %d\n", inode->i_num));

    for(i = 0; i < NR_TZONES; i++){
        zone_id = inode->i_zone[i];
        if(zone_id > 0){
            if(is_indirect_zone || i < NR_DIRECT_ZONE){
                // KDEBUG(("releasing block %d for %d\n", zone_id, inode->i_num));
                release_block(zone_id, id);
                inode->i_zone[i] = 0;
            }else{
                struct inode* indirect_zone = get_inode(zone_id, id);
                if(indirect_zone){
                    put_inode(indirect_zone, false);
                    _release_inode(indirect_zone, true);
                }
            }
        }
    }
    
    // assumping inum is smaller than 1024 for simplicity
    imap = get_block_buffer(sb->s_inode_tablenr, id);
    bitmap_clear_bit((unsigned int*)imap->block, BLOCK_SIZE_DWORD, inum);
    sb->s_inode_inuse -= 1;
    sb->s_free_inodes += 1;
    put_block_buffer_dirt(imap);

    memset(inode, 0, sizeof(struct inode));
    return OK;
}


int fill_dirent(inode_t* ino, struct winix_dirent* curr, const char* string){
    mode_t mode = ino->i_mode;
    char32_strlcpy(curr->dirent.d_name, string, DIRNAME_LEN);
    curr->dirent.d_ino = ino->i_num;

    if(mode & S_IFDIR){
        curr->dirent.d_type = DT_DIR;
    }else if(mode & S_IFCHR){
        curr->dirent.d_type = DT_CHR;
    }else if(mode & S_IFREG){
        curr->dirent.d_type = DT_REG;
    }else if(mode & S_IFBLK){
        curr->dirent.d_type = DT_BLK;
    }
    return OK;
}

int init_dirent(inode_t* dir, inode_t* ino){
    struct winix_dirent* curr;
    struct block_buffer* buf;
    block_t bnr;

    if(!S_ISDIR(ino->i_mode))
        return -ENOTDIR;
    bnr = ino->i_zone[0];
    if(bnr == 0)
        return -EINVAL;

    buf = get_block_buffer(bnr, ino->i_dev);
    curr = (struct winix_dirent*)buf->block;
    fill_dirent( ino, curr, ".");
    curr++;
    fill_dirent(dir, curr, "..");
    put_block_buffer_dirt(buf);
    return OK;
}



int add_inode_to_directory(struct proc* who, struct inode* dir, struct inode* ino, char* string){
    struct winix_dirent* curr;
    struct dirent_iterator iter;
    int ret = 0;

    if(!(S_ISDIR(dir->i_mode)))
        return -EINVAL;
    if(!has_file_access(who, dir, W_OK))
        return -EACCES;
    if(strlen(string) >= DIRNAME_LEN)
        return -ENAMETOOLONG;

    iter_dirent_init(&iter, dir, 0, 0);
    while(true){
        if(!iter_dirent_has_next(&iter)){
            ret = iter_dirent_alloc(&iter);
            if(ret < 0)
                break;
            dir->i_size += BLOCK_SIZE;
        }
        curr = iter_dirent_get_next(&iter);
        if(curr->dirent.d_name[0] == '\0'){
            fill_dirent(ino, curr, string);
            curr->dev = ino->i_dev->dev_id;
            ino->i_nlinks += 1;
            iter.buffer->b_dirt = true;
            ret = 0;
            break;
        }
    }

    iter_dirent_close(&iter);
    return ret;
}

int remove_inode_from_dir(struct proc* who, struct inode* dir, struct inode* target, char* name){
    struct winix_dirent* curr;
    struct dirent_iterator iter;
    int ret = -ENOENT;

    if(!has_file_access(who, dir, W_OK))
        return -EACCES;

    iter_dirent_init(&iter, dir, 0, 0);
    while(iter_dirent_has_next(&iter)){
        curr = iter_dirent_get_next(&iter);
        if(curr->dirent.d_ino == target->i_num && char32_strcmp(curr->dirent.d_name, name) == 0){
            curr->dirent.d_name[0] = '\0';
            curr->dirent.d_ino = 0;
            iter.buffer->b_dirt = true;
            target->i_nlinks -= 1;
            ret = 0;
            break;
        }
    }

    iter_dirent_close(&iter);
    return ret;
}

int iter_zone_init(struct zone_iterator* iter, struct inode* inode, int zone_idx){
    iter->i_inode = inode;
    inode->i_count++;
    iter->i_zone_idx = zone_idx;
    return OK;
}

zone_t _iter_get_current_zone(struct zone_iterator* iter, bool create_inode, bool create_block){
    int ino_iter, ino_rem, indirect_idx;
    zone_t ret = 0;
    zone_t *pos;
    struct inode* indirect_ino = NULL, *inode;
    struct device* dev = iter->i_inode->i_dev;
    inode = iter->i_inode;

    if (iter->i_zone_idx >= MAX_ZONES )
        return 0;
    if (iter->i_zone_idx < NR_DIRECT_ZONE)
    {
        pos = &iter->i_inode->i_zone[iter->i_zone_idx];
    }
    else
    {
        // if we are in indirect zone
        indirect_idx = iter->i_zone_idx - NR_DIRECT_ZONE;
        ino_iter = indirect_idx / NR_TZONES;
        ino_rem = indirect_idx % NR_TZONES;
        pos = &iter->i_inode->i_zone[NR_DIRECT_ZONE + ino_iter];
        // if the zone number in indirect zone is 0,
        // create inode if create_inode is set
        if (*pos == 0){
            if(!create_inode)
                goto final;
            
            indirect_ino = alloc_inode(dev, dev);
            if(!indirect_ino){
                ret = -ENOSPC;
                goto final;
            }
            indirect_ino->i_flags |= INODE_FLAG_ZONE | INODE_FLAG_DIRTY;
            *pos = (zone_t)indirect_ino->i_num;
        }else{
            indirect_ino = get_inode(*pos, inode->i_dev);
            if (!indirect_ino){
                kwarn("inode %d indirect ino %d not found\n", iter->i_inode->i_num, *pos);
                ret = -EINVAL;
                goto final;
            }
        }
        pos = &indirect_ino->i_zone[ino_rem];
        inode = indirect_ino;
    }
    
    if(create_block){
        if (*pos){ // if creating block but this zone already has block
            ret = -EINVAL;
            goto final;
        }
        ret = alloc_block(inode, inode->i_dev);
        if(ret < 0)
            goto final;
        *pos = (zone_t)ret;
    }
    ret = *pos;

final:
    if (indirect_ino)
        put_inode(indirect_ino, create_inode || create_block);
    return ret;
}

bool iter_zone_has_next(struct zone_iterator* iter){
    return (bool)_iter_get_current_zone(iter, false, false);
}

zone_t iter_zone_get_next(struct zone_iterator* iter){
    zone_t zone = _iter_get_current_zone(iter, false, false);
    if( zone)
        iter->i_zone_idx++;
    return zone;
}

int iter_zone_alloc(struct zone_iterator* iter){
    if (iter->i_zone_idx >= MAX_ZONES )
        return -EFBIG;
    return _iter_get_current_zone(iter, true, true);
}

int iter_zone_close(struct zone_iterator* iter){
    iter->i_zone_idx = 0;
    iter->i_inode->i_count--;
    return 0;
}

struct winix_dirent* _iter_dirent_get_current(struct dirent_iterator* iter){
    zone_t zone;
    struct block_buffer* buffer;
    if(iter->dirent + 1 >= iter->dirent_end){
        if(!iter_zone_has_next(&iter->zone_iter))
            return NULL;
        zone = iter_zone_get_next(&iter->zone_iter);
        buffer = get_block_buffer(zone, iter->zone_iter.i_inode->i_dev);
        iter->dirent = (struct winix_dirent*)buffer->block;
        iter->dirent_end = (struct winix_dirent* )&buffer->block[BLOCK_SIZE];
        if(iter->buffer)
            put_block_buffer(iter->buffer);
        iter->buffer = buffer;
    }
    return iter->dirent;
}

int iter_dirent_init(struct dirent_iterator* iter, struct inode* inode, int zone_idx, int dir_idx){
    iter->buffer = NULL;
    iter->dirent_end = NULL;
    iter_zone_init(&iter->zone_iter, inode, zone_idx);
    iter->dirent = _iter_dirent_get_current(iter);
    iter->dirent += dir_idx;
    return 0;
}

bool iter_dirent_has_next(struct dirent_iterator* iter){
    if(iter->dirent + 1 >= iter->dirent_end)
        if(!iter_zone_has_next(&iter->zone_iter))
            return false;
    
    return true;
}

struct winix_dirent* iter_dirent_get_next(struct dirent_iterator* iter){
    iter->dirent = _iter_dirent_get_current(iter);
    return iter->dirent++;
}

int iter_dirent_alloc(struct dirent_iterator* iter){
    int ret = iter_zone_alloc(&iter->zone_iter);
    if (ret >= 0)
        return 0;
    return ret;
}

int iter_dirent_close(struct dirent_iterator* iter){
    if(iter->buffer){
        put_block_buffer(iter->buffer);
        iter->buffer = NULL;
    }
    
    return iter_zone_close(&iter->zone_iter);
}




void init_inode(){
    inode_t* rep;
    int i;
    for(i = 0; i < NR_INODES; i++){
        rep = &inode_table[i];
        rep->i_num = 0;
        rep->i_count = 0;
    }
}


