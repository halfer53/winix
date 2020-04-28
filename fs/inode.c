#include "fs.h"

inode_t inode_table[NR_INODES];

struct inode* get_free_inode_slot(){
    inode_t* rep;
    for(rep = inode_table; rep < &inode_table[NR_INODES]; rep++ ){
        if(rep->i_num == 0){
            memset(rep, 0, sizeof(struct inode));
            return rep;
        }
    }

    for(rep = &inode_table[NR_INODES]; rep >= inode_table; rep-- ){
        if(rep->i_count <= 0){
            memset(rep, 0, sizeof(struct inode));
            return rep;
        }
    }
    return NULL;
}

bool is_valid_inode_num(int num, struct device* id){
    struct superblock* sb = get_sb(id);
    if(num <= 0 || num >= NR_INODES)
        return false;

    int inodes_nr = sb->s_inode_per_block * sb->s_inode_tablenr;
    return num <= inodes_nr;
}

bool is_inode_in_use(int num, struct device* id){
    bool ret;
    if(!is_valid_inode_num(num, id)){
        return false;
    }
    struct superblock* sb = get_sb(id);
    block_buffer_t *buf = get_imap(sb->s_inodemapnr, id);
    ret = is_bit_on(buf->block, sb->s_inodemap_size, num);
    return ret;
}

block_t alloc_block(inode_t *ino, struct device* id){
    struct superblock* sb = get_sb(id);
    block_buffer_t *bmap = get_block_buffer(sb->s_blockmapnr, id);
    block_t bmap_end = sb->s_blockmapnr + (sb->s_blockmap_size / BLOCK_SIZE);
    block_t bnr = bmap->b_blocknr;
    int free_bit = -1;
    int ret;

    while(bmap->b_blocknr < bmap_end){
        free_bit = bitmap_search_from(bmap->block, BLOCK_SIZE, 0, 1);
        if(free_bit > 0){
            bitmap_set_bit(bmap->block, BLOCK_SIZE, free_bit);
            sb->s_block_inuse += 1;
            sb->s_free_blocks -= 1;
            put_block_buffer_immed(bmap, id);
            KDEBUG(("Alloc block %d for inode %d\n", free_bit, ino->i_num));
            return free_bit;
        }
        put_block_buffer(bmap);
        bnr++;
        bmap = get_block_buffer(bnr, id);
    }
    KDEBUG(("no free block id found for dev %d", id->dev_id));
    put_block_buffer(bmap);
    return ENOSPC;
}

int release_block(block_t bnr, struct device* id){
    struct superblock* sb = get_sb(id);
    block_t bmap_nr = sb->s_blockmapnr + (bnr / BLOCK_SIZE);
    block_buffer_t *bmap;
    int ret;
    if(bnr > sb->s_blockmap_size){
        KDEBUG(("Invalid block id %d", bnr));
        return -1;
    }

    bmap = get_block_buffer(bmap_nr, id);

    bitmap_clear_bit(bmap->block, BLOCK_SIZE, bnr);
    sb->s_block_inuse -= 1;
    sb->s_free_blocks += 1;
    return put_block_buffer(bmap);
}

blkcnt_t get_inode_blocks(struct inode* ino){
    blkcnt_t ret = 0;
    int i;
    for(i = 0; i < NR_TZONES; i++){
        if(ino->i_zone[i] > 0){
            ret++;
        }
    }
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
    ino->i_total_size = get_inode_total_size_word(ino);
    if(sb){
        bnr = (num * sb->s_inode_size) / BLOCK_SIZE;
        if(bnr * BLOCK_SIZE > sb->s_inode_table_size){
            return ERR;
        }
        ino->i_ndblock = bnr;
    }
    return OK;
}

inode_t* read_inode(int num, struct device* id){
    struct superblock* sb = get_sb(id);
    block_t inode_block_nr = (num * sb->s_inode_size) / BLOCK_SIZE;
    int offset = (num * sb->s_inode_size) % BLOCK_SIZE;
    block_t blocknr = sb->s_inode_tablenr + inode_block_nr;
    block_buffer_t *buffer;
    inode_t *inode = get_free_inode_slot();

    if(!is_inode_in_use(num, id))
        return NULL;

    buffer = get_block_buffer(blocknr, id);
    memcpy(inode, &buffer->block[offset], INODE_DISK_SIZE_BYTE);
    inode->i_count += 1;
    init_inode_non_disk(inode, num, id, sb);
    put_block_buffer(buffer);
    return inode;
}

inode_t* get_inode(int num, struct device* id){
    inode_t* rep;
    for(rep = &inode_table[0]; rep < &inode_table[NR_INODES]; rep++ ){
        if(rep->i_num == num){
            rep->i_count += 1;
            return rep;
        }
    }

    rep = read_inode(num, id);
    return rep;
}


int put_inode(inode_t *inode, bool is_dirty){
    struct superblock* sb;
    int i = 0;
    int inum;
    int inode_block_offset;
    if(!inode)
        return EINVAL;
    inode->i_count -= 1;
    if(!is_dirty)
        return OK;
    sb = get_sb(inode->i_dev);
    inum = inode->i_num;
    inode_block_offset = (inum * sb->s_inode_size) % BLOCK_SIZE;;
    block_buffer_t *buffer = get_block_buffer(inode->i_ndblock, inode->i_dev);
    memcpy(buffer->block + inode_block_offset, inode, INODE_DISK_SIZE_BYTE);
    flush_inode_zones(inode);
    return put_block_buffer_immed(buffer, inode->i_dev);
}


inode_t* alloc_inode(struct device* id){
    struct superblock* sb = get_sb(id);
    int inum = 0, ret = 0;
    block_t imap_end = 0, bnr = 0;
    block_buffer_t *imap, *ino_table_buffer;
    inode_t *inode;
    bool found = false;


    imap = get_imap(inum, id);
    imap_end = imap->b_blocknr + (sb->s_blockmap_size / BLOCK_SIZE);
    while(imap->b_blocknr < imap_end){
        inum = bitmap_search_from(imap->block, BLOCK_SIZE, 0, 1);
        if(inum > 0){
            found = true;
            break;
        }
        put_block_buffer(imap);
        bnr++;
        imap = get_block_buffer(bnr, id);
    }
    if(!found){
        put_block_buffer(imap);
        return NULL;
    }
    bitmap_set_bit(imap->block, BLOCK_SIZE, inum);
    put_block_buffer_immed(imap, id);

    inode = get_free_inode_slot();
    init_inode_non_disk(inode, inum, id, sb);

    sb->s_free_inodes -= 1;
    sb->s_inode_inuse += 1;
    return inode;
}


int release_inode(inode_t *inode){
    struct device* id = inode->i_dev;
    int inum = inode->i_num;
    struct superblock* sb = get_sb(id);
    block_t imap_nr = sb->s_inodemapnr + (inum * sb->s_inode_size / BLOCK_SIZE);
    block_buffer_t *imap;
    block_t zone_id;
    int i = 0;
    int ret;
    if(!is_inode_in_use(inum, id)){
        KDEBUG((" inode id %d not in use, cannot be released", inum));
        return EINVAL;
    }

    for(i = 0; i < NR_TZONES; i++){
        zone_id = inode->i_zone[i];
        if(zone_id > 0){
            release_block(zone_id, id);
            inode->i_zone[i] = 0;
        }
    }

    imap = get_block_buffer(imap_nr, id);

    bitmap_clear_bit(imap->block, BLOCK_SIZE, inum);
    sb->s_inode_inuse -= 1;
    sb->s_free_inodes += 1;
    put_block_buffer_immed(imap, id);

    memset(inode, 0, sizeof(struct inode));
    return OK;
}

int fill_dirent(inode_t* ino, struct dirent* curr, char* string){
    mode_t mode = ino->i_mode;
    strcpy(curr->d_name, string);
    curr->d_ino = ino->i_num;

    if(mode & S_IFDIR){
        curr->d_type = DT_DIR;
    }else if(mode & S_IFCHR){
        curr->d_type = DT_CHR;
    }else if(mode & S_IFREG){
        curr->d_type = DT_REG;
    }
    return OK;
}

int init_dirent(inode_t* dir, inode_t* ino){
    struct dirent* curr;
    struct block_buffer* buf;
    block_t bnr;
    int i;

    for (int i = 0; i < NR_TZONES; ++i) {
        if((bnr = ino->i_zone[i]) > 0){
            buf = get_block_buffer(bnr, ino->i_dev);
            curr = (struct dirent*)buf->block;
            fill_dirent( ino, curr, ".");
            curr++;
            fill_dirent(dir, curr, "..");
            put_block_buffer_dirt(buf);
            return OK;
        }
    }
    return ENOSPC;
}



int add_inode_to_directory( struct inode* dir, struct inode* ino, char* string){
    int i, j, ret;
    block_t bnr;
    struct dirent* curr, *end;
    struct block_buffer* buf;

    if(!(dir->i_mode & S_IFDIR))
        return EINVAL;
    if(!(dir->i_mode & O_WRONLY))
        return EACCES;

    init_dirent(dir, ino);
    for(i = 0; i < NR_TZONES; i++){
        bnr = dir->i_zone[i];
        if(bnr == 0){
            bnr = alloc_block(dir, dir->i_dev);
            dir->i_zone[i] = bnr;
            dir->i_total_size += BLOCK_SIZE;
        }
        buf = get_block_buffer(bnr, dir->i_dev);
        end = (struct dirent*)&buf->block[BLOCK_SIZE];
        curr = (struct dirent*)buf->block;
        while(curr < end){
            if(curr->d_name[0] == '\0'){
                fill_dirent(ino, curr, string);
                put_block_buffer_dirt(buf);
                ino->i_nlinks += 1;
                return OK;
            }
            curr++;
        }
        put_block_buffer(buf);
    }
    return ENOSPC;
}

int remove_inode_from_dir(struct inode* dir, struct inode* target){
    int i;
    block_t bnr;
    struct block_buffer* buf;
    struct dirent *curr, *end;
    int ret;
    for(i = 0; i < NR_TZONES; i++){
        bnr = dir->i_zone[i];
        if(bnr == 0)
            continue;
        buf = get_block_buffer(bnr, dir->i_dev);
        end = (struct dirent*)&buf->block[BLOCK_SIZE];
        curr = (struct dirent*)buf->block;
        while(curr < end){
            if(curr->d_ino == target->i_num){
                curr->d_name[0] = '\0';
                curr->d_ino = 0;
                put_block_buffer_dirt(buf);
                target->i_nlinks -= 1;
                return OK;
            }
            curr++;
        }
        put_block_buffer(buf);
    }
    return ENOENT;
}



void init_inode(){
    inode_t* rep;
    for(rep = &inode_table[0]; rep < &inode_table[NR_INODES]; rep++ ){
        rep->i_num = 0;
    }
}