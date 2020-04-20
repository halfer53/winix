#include "fs.h"

inode_t inode_table[NR_INODES];

bool is_valid_inode_num(int num, dev_t id){
    struct superblock* sb = get_sb(id);
    int inodes_nr = sb->s_inode_per_block * sb->s_inode_tablenr;
    return num <= inodes_nr;
}

bool is_inode_in_use(int num, dev_t id){
    bool ret;
    if(!is_valid_inode_num(num, id)){
        return false;
    }
    struct superblock* sb = get_sb(id);
    block_buffer_t *buf = get_imap(sb->s_inodemapnr, id);
    ret = is_bit_on(buf->block, sb->s_inodemap_size, num);
    return ret;
}

block_t alloc_block(dev_t id){
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
            XDEBUG(("Alloc block %d\n", free_bit));
            return free_bit;
        }
        put_block_buffer(bmap);
        bnr++;
        bmap = get_block_buffer(bnr, id);
    }
    XDEBUG(("no free block id found for dev %d", id));
    put_block_buffer(bmap);
    return -1;

}

int release_block(block_t bnr, dev_t id){
    struct superblock* sb = get_sb(id);
    block_t bmap_nr = sb->s_blockmapnr + (bnr / BLOCK_SIZE);
    block_buffer_t *bmap;
    int ret;
    if(bnr > sb->s_blockmap_size){
        XDEBUG(("Invalid block id %d", bnr));
        return -1;
    }

    bmap = get_block_buffer(bmap_nr, id);

    ret = bitmap_clear_bit(bmap->block, BLOCK_SIZE, bnr);
    if(ret)
        return ret;
    sb->s_block_inuse -= 1;
    sb->s_free_blocks += 1;
    return put_block_buffer(bmap);
}

inode_t* read_inode(int num, dev_t id){
    struct superblock* sb = get_sb(id);
    block_t inode_block_nr = (num * sb->s_inode_size) / BLOCK_SIZE;
    int offset = (num * sb->s_inode_size) % BLOCK_SIZE;
    block_t blocknr = sb->s_inode_tablenr + inode_block_nr;
    block_buffer_t *buffer;
    inode_t *inode = NIL_INODE;

    for(inode = &inode_table[0]; inode < &inode_table[NR_INODES]; inode++){
        if(inode->i_num == 0)
            break;
    }

    if(inode == NIL_INODE)
        return NIL_INODE;

    buffer = get_block_buffer(blocknr, id);
    memcpy(inode, &buffer->block[offset], sizeof(struct inode));
    inode->i_sb = sb;
    inode->i_dev = id;
    inode->i_num = num;
    put_block_buffer(buffer);
    return inode;
}

inode_t* get_inode(int num, dev_t id){
    inode_t* rep;
    int i=0;
    block_buffer_t *imap;
    char val;
    struct superblock* sb = get_sb(id);
    for(rep = &inode_table[0]; rep < &inode_table[NR_INODES]; rep++ ){
        if(rep->i_num == num)
            return rep;
    }
    
    if(is_inode_in_use(num, id)){
        rep = read_inode(num, id);
        rep->i_num = num;
        return rep;
    }
    return NULL;
}


int put_inode(inode_t *inode, dev_t id){
    struct superblock* sb = get_sb(id);
    int i = 0;
    int inum = inode->i_num;
    block_t blocknr;
    struct block_buffer* buf;
    int inode_block_offset = (inum * sb->s_inode_size) % BLOCK_SIZE;

    block_buffer_t *buffer = get_block_buffer(inode->i_ndblock, id);
    memcpy(buffer->block + inode_block_offset, inode, sizeof(struct inode));

    flush_inode_zones(inode);
    return put_block_buffer_immed(buffer, id);
}


inode_t* alloc_inode(dev_t id){
    struct superblock* sb = get_sb(id);
    int inum = 0, ret = 0;
    block_t iblock = sb->s_nblock;
    block_t imap_end = 0, bnr = 0;
    block_buffer_t *imap, *ino_table_buffer;
    inode_t *inode;
    bool found = false;

    for(inode = &inode_table[0]; inode < &inode_table[NR_INODES]; inode++){
        if(inode->i_count == 0){ // if inode is free
            found = true;
            break;
        }
    }
    if(!found)
        return NULL;

    imap = get_imap(inum, id);
    imap_end = imap->b_blocknr + (sb->s_blockmap_size / BLOCK_SIZE);
    found = false;
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

    bnr = alloc_block(id);
    inode->i_zone[0] = bnr;

    ino_table_buffer = get_inode_table(inum, id);

    inode->i_num = inum;
    inode->i_dev = id;
    inode->i_sb = sb;
    inode->i_ndblock = ino_table_buffer->b_blocknr;
    inode->i_size = BLOCK_SIZE;
    put_block_buffer(ino_table_buffer);

    sb->s_free_inodes -= 1;
    sb->s_inode_inuse += 1;
    return inode;
}


int release_inode(inode_t *inode){
    dev_t id = inode->i_dev;
    int inum = inode->i_num;
    struct superblock* sb = get_sb(id);
    block_t imap_nr = sb->s_inodemapnr + (inum * sb->s_inode_size / BLOCK_SIZE);
    block_buffer_t *imap;
    block_t zone_id;
    int i = 0;
    int ret;
    if(!is_inode_in_use(inum, id)){
        XDEBUG((" inode id %d not in use, cannot be released", inum));
        return -1;
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
    ret = put_block_buffer_immed(imap, id);

    memset(inode, 0, sizeof(struct inode));
    return ret;
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
            fill_dirent(ino, curr, ".");
            curr++;
            fill_dirent(dir, curr, "..");
            put_block_buffer_dirt(buf);
            return OK;
        }
    }
    return ERR;
}



int add_inode_to_directory( inode_t* dir, inode_t* ino, char* string){
    if(!(dir->i_mode & S_IFDIR))
        return ERR;
    int i, j, ret;
    block_t bnr;
    struct dirent* curr, *end;
    struct superblock* sb = get_sb(dir->i_dev);
    struct block_buffer* buf;

    init_dirent(dir, ino);
    for(i = 0; i < NR_TZONES; i++){
        bnr = dir->i_zone[i];
        if(bnr == 0){
            bnr = alloc_block(dir->i_dev);
            dir->i_zone[i] = bnr;
        }
        buf = get_block_buffer(bnr, dir->i_dev);
        sb = get_sb(dir->i_dev);
        end = (struct dirent*)&buf->block[BLOCK_SIZE];
        curr = (struct dirent*)buf->block;
        while(curr < end){
            if(curr->d_name[0] == '\0'){
                ret = fill_dirent(ino, curr, string);
                put_block_buffer_immed(buf, dir->i_dev);
                return ret;
            }
            curr++;
        }
        put_block_buffer(buf);
    }
    return ERR;
}

void init_inode(){
    inode_t* rep;
    for(rep = &inode_table[0]; rep < &inode_table[NR_INODES]; rep++ ){
        rep->i_num = 0;
    }
}