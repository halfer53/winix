#include "fs.h"
inode_t inode_table[NR_INODES];

static unsigned int sector_buffer[SECTOR_SIZE];

inode_t* get_inode(int num){
    register inode_t* rep;
    register int i=0;
    for(rep = &inode_table[0]; rep < &inode_table[NR_INODES]; rep++ ){
        if(rep->i_num == num)
            return rep;
    }
    return NULL;
}


int put_inode(inode_t *inode){
    int inum = inode->i_num;
    sector_t sectornr = inode->i_ndsector;
    unsigned int *buf = sector_buffer;
    int inode_sector_offset = inum % 4 * 128; 
    unsigned int *bak;
    //offset in terms of byte index, each sector in inode table contains four inode
    int i;
    if(dev_io_sector(buf,sectornr,DEV_READ) == 0){
        buf += inode_sector_offset; 
        bak = buf;
        int2hexstr(buf,inode->i_mode,8);
        buf+=8;
        int2hexstr(buf,inode->i_nlinks,8);
        buf+=8;
        int2hexstr(buf,inode->i_uid,8);
        buf+=8;
        int2hexstr(buf,inode->i_gid,8);
        buf+=8;
        int2hexstr(buf,inode->i_size,8);
        buf+=8;
        int2hexstr(buf,inode->i_atime,8);
        buf+=8;
        int2hexstr(buf,inode->i_mtime,8);
        buf+=8;
        int2hexstr(buf,inode->i_ctime,8);
        buf+=8;
        for(i=0; i< 8; i++){
            hex2str(buf,inode->i_zone[i]);
            buf+=8;
        }
        if(buf - bak > 128){
            printf("oversize on put inode buffer");
        }
        return dev_io(buf,sectornr,DEV_READ);
    }
    return 2;//error on device driver
}


inode_t* next_free_inode(){
    int inum = sb->s_ninode; //next free inode
    block_t iblock = sb->s_nblock;
    unsigned int *buf = sector_buffer;
    inode_t *inode = NIL_INODE;
    int i;

    //TODO: if inum is negative, (no inode available), return NULL
    for(inode = &inode_table[0]; inode < &inode_table[NR_INODES]; inode++){
        if(inode->i_num == 0){ //if inode is free
            break;
        }
    }
    if(inode->i_num)
        put_inode(inode);

    //allocate new inode and block
    if(dev_io_sector(buf,sb->s_inodemapnr,DEV_READ) == 0){
        buf[inum/32] |= (0x80000000) >> (inum%32);
        if(dev_io_sector(buf,sb->s_inodemapnr, DEV_WRITE) == 0){
            if(dev_io_sector(buf,sb->s_blockmapnr,DEV_READ) == 0){
                buf[iblock/32] |= (0x80000000) >> (iblock%32);
                if(dev_io_sector(buf,sb->s_blockmapnr,DEV_WRITE) == 0){
                    inode->i_zone[0] = iblock;
                    inode->i_num = inum;
                    inode->i_ndsector = sb->s_inode_tablenr + inum / sb->inode_per_sector;
                    return inode;
                }
            }
        }
    }
    return NIL_INODE;
}




inode_t *alloc_inode(){
    return next_free_inode();
}

void init_inodetable(){
    int i;
    for( i=0; i< NR_INODES ; i++){
        open_inodes[i] = NIL_INODE;
    }
}