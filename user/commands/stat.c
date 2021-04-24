#include <sys/syscall.h>

void int2str(int value, int i, char* output){
    int j;
    while(i){
        j = (value / i) % 10;
        *output++ = '0' + j;
        i /= 10;
    }
}

int main(int argc, char* argv[]){
    int ret, size, mod;
    struct stat statbuf;
    char buffer[10];
    char *buf = buffer;
    char* path = argv[1];
    if(argc != 2)
        return 1;
    ret = stat(path, &statbuf);
    if(ret){
        perror("stat");
        return 1;
    }
    size = statbuf.st_size / 4096;
    mod = statbuf.st_size % 4096;
    int2str(size, 10, buf);
    if(*buf == '0'){
        *buf = ' ';
    }
    buf += 2;
    *buf++ = '.';
    int2str(mod, 100, buf);
    buf += 2;
    *buf = '\0';

    printf("File: %s\nNum: %d\nSize: %sKB\nBlocks: %d\nAccess: 0x%x\n", 
        path, statbuf.st_ino, buffer, statbuf.st_blocks, statbuf.st_mode & 0x777);
    return 0;
}

