#include "fs.h"
#include "inode.h"

inode_t inode_table[NR_INODES];


inode_t* read_inode(int num){
    block_t blocknr = sb->s_inode_tablenr + num / sb->s_inode_per_block;
    int offset = (num) % sb->s_inode_per_block * sb->s_inode_size;
    char *start;
    int *ino_buf;
    buf_t *buffer;
    inode_t *inode = NIL_INODE;

    for(inode = &inode_table[0]; inode < &inode_table[NR_INODES]; inode++){
        if(inode->i_num == 0)
            break;
    }
    
    if(inode == NIL_INODE)
        return NIL_INODE;

    // printf("free inode found\n");
    
    buffer = get_block(blocknr);
    ino_buf = (int *)inode;
    for(start = &buffer->block[offset]; start < &buffer->block[offset] + sb->s_inode_size; start+=8, ino_buf++  ){
        *ino_buf = hexstr2int(start,8);
    }
    return inode;
}

inode_t* get_inode(int num){
    inode_t* rep;
    int i=0;
    buf_t *imap;
    char val;
    for(rep = &inode_table[0]; rep < &inode_table[NR_INODES]; rep++ ){
        if(rep->i_num == num)
            return rep;
    }
    
    imap = get_imap();
    val = imap->block[num/32];
    if((0x80000000 >> (num%32))){ // if it is a inode
        rep = read_inode(num);
        rep->i_num = num;
        return rep;
    }
    
    return NULL;
}






int put_inode(inode_t *inode){

    int inum = inode->i_num;
    block_t blocknr = inode->i_ndblock;
    int inode_block_offset = (inum) % sb->s_inode_per_block * sb->s_inode_size;
    char *bak;

    buf_t *buffer = get_block(blocknr);
    char *buf = &buffer->block[0];
    // offset in terms of byte index, each sector in inode table contains four inode
    int i;

    buf += inode_block_offset; 
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
    int2hexstr(buf,inode->i_ctime,8); //64 bytes so far
    buf+=8;
    for(i=0; i< NR_INODES; i++){
        int2hexstr(buf,inode->i_zone[i],8);
        buf+=8;
    }
    if(buf - bak > sb->s_inode_per_block){
        printf("oversize on put inode buffer");
    }
    return put_block(buffer,WRITE_IMMED);
}


inode_t* alloc_inode(){
    int inum = sb->s_ninode; // next free inode
    block_t iblock = sb->s_nblock;
    buf_t *imap, *bmap;
    inode_t *inode;
    int i;

    // TODO: if inum is negative, (no inode available), return NULL
    for(inode = &inode_table[0]; inode < &inode_table[NR_INODES]; inode++){
        if(inode->i_num == 0){ // if inode is free
            break;
        }
    }

    imap = get_imap();

    // allocate new inode and block

    imap->block[inum/32] |= (0x80000000) >> (inum%32);
    if(put_imap(imap) == 0){

        bmap = get_bmap();
        bmap->block[iblock/32] |= (0x80000000) >> (iblock%32);
        if(put_bmap(bmap) == 0){
            inode->i_zone[0] = iblock;
            inode->i_num = inum;
            inode->i_ndblock = sb->s_inode_tablenr + ( inum / sb->s_inode_per_block);
            sb->s_ninode += 1;
            sb->s_nblock += 1;
            return inode;
        }
        
    }
    return NIL_INODE;
}


void free_inode(inode_t *inode){
    
}

void init_inode(){
    inode_t* rep;
    for(rep = &inode_table[0]; rep < &inode_table[NR_INODES]; rep++ ){
        rep->i_num = 0;
    }
}