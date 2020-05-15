#include <stdio.h>
#include <sys/syscall.h>

int main(int argc, char* argv[]){
    int ret;
    struct stat statbuf;
    char* path = argv[1];
    if(argc != 2)
        return -1;
    ret = stat(path, &statbuf);
    if(ret){
        perror("stat");
        return -1;
    }
    printf("File: %s\nSize: %d\nBlocks: %d\nAccess: 0x%x\n", 
        path, statbuf.st_size, statbuf.st_blocks, statbuf.st_mode & 0x777);
    return 0;
}

