#include "dev.h"
#include "makefs.h"
#include "fs.h"

int dev_io_sector(unsigned int *buf, sector_t sectornr, mode_t mode){
    
}

int dev_io(unsigned int *buf, block_t blocknr,mode_t mode){
    int i = blocknr * BLOCK_SIZE;
    char *disk_buf = &disk;
    i = align_sector(i);
    unsigned int *limit = buf + BLOCK_SIZE;
    if(mode == DEV_READ){
        for(; buf < limit; buf++, disk_buf++){
            *buf = (char)*disk_buf;
        }
        // memcpy(buf,disk[start],SECTOR_SIZE);
    }else if (mode == DEV_WRITE){
        
        for(; buf < limit; buf++, disk_buf++){
            *disk_buf = (unsigned int)*buf;
        }
        // memcpy(buf,disk[start,SECTOR_SIZE]);
    }
}


