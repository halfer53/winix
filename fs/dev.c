#include "dev.h"
#include "makefs.h"
#include "fs.h"

int dev_io_sector(unsigned int *buf, sector_t sectornr, mode_t mode){
    int i = sectornr * SECTOR_SIZE;
    i = align_sector(i);
    unsigned int *limit = buf + SECTOR_SIZE;
    if(mode == DEV_READ){
        for(; buf < limit; buf++){
            *buf = disk[i];
        }
        // memcpy(buf,disk[start],SECTOR_SIZE);
    }else if (mode == DEV_WRITE){
        
        for(; buf < limit; buf++){
            disk[i] = *buf;
        }
        // memcpy(buf,disk[start,SECTOR_SIZE]);
    }
}

int dev_io(unsigned int *buf, block_t blocknr,mode_t mode){
    int sector_num = blocknr * 2;
    if(dev_io_sector(buf,sector_num,mode) ==0){
        if(dev_io_sector(buf+SECTOR_SIZE,sector_num+1,mode) == 0){
            return 0;
        }
    }
    return 1;
}


