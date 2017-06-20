#include "fs.h"

int rw_file(int fd, void *buf, size_t count, int flag){
    // char *pbuf = get_physical_addr(buf);
    int ret, r;
    int open_slot, pos;
    filp_t *filp;
    int b,off, len;
    buf_t *buffer;
    block_t blocknr, endblocknr;
    inode_t *ino;
    char c;
    ret = get_fd(current_proc, 0, &open_slot, &filp);

    ino = filp->filp_ino;
    if(ret != 0)
        return ENFILE;

    blocknr = filp->filp_pos / BLOCK_SIZE;
    off = filp->filp_pos % BLOCK_SIZE;
    endblocknr = (filp->filp_pos + count) / BLOCK_SIZE;
    

    for( ; blocknr <= endblocknr; blocknr++){
        buffer = get_block(blocknr);
        len = BLOCK_SIZE - off;
        c = buffer->block[off];
        /* Read or write 'chunk' bytes. */
		// r = rw_chunk(ino,  off, blocknr, endblocknr- blocknr, falgs);

		if (r != OK) break;	/* EOF reached */
        off = 0;
    }
    return 0;
}

int do_read(int fd, void *buf, size_t count){
    rw_file(fd, buf,count, READING);
}

int do_write(int fd, void *buf, size_t count){
    rw_file(fd, buf, count, WRITING);
}

