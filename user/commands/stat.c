#include <sys/syscall.h>

int main(int argc, char* argv[]){
    int ret;
    struct stat statbuf;
    char* path = argv[1];
    int div, rem, size;
    if(argc != 2)
        return 1;
    ret = stat(path, &statbuf);
    if(ret){
        perror("stat");
        return 1;
    }
    size = statbuf.st_size;
    div = size / 1024;
    rem = size % 1024;
    rem = rem > 1000 ? 999 : rem;

    printf("File: %s\nNum: %d\nSize: %d.%.2dKB\nBlocks: %d\nAccess: 0x%x\n", 
        path, statbuf.st_ino, div, rem, statbuf.st_blocks, statbuf.st_mode & 0x777);
    return 0;
}

