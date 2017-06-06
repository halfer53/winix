#include "dev.h"
#include "makefs.h"
#include "fs.h"
#include <string.h>

int dev_io_sector(char *buf, sector_t sectornr, mode_t mode){
    int start = sectornr * SECTOR_SIZE;
    start = align_sector(start);
    if(mode == DEV_READ){
        memcpy(buf,disk[start],SECTOR_SIZE);
    }else if (mode == DEV_WRITE){
        memcpy(buf,disk[start,SECTOR_SIZE]);
    }
}

int dev_io(char *buf, block_t blocknr,mode_t mode){
    int sector_num = blocknr * 2;
    if(dev_io_sector(buf,sector_num,mode) ==0){
        if(dev_io_sector(buf+SECTOR_SIZE,sector_num+1,mode) == 0){
            return 0;
        }
    }
    return 1;
}


