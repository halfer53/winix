#include <stdio.h>
#include <sys/syscall.h>
#include <sys/fcntl.h>


int main(int argc, char *argv[]){
    int fd, ret;
    char buf[256];
    if(argc < 2)
        return -1;
    fd = open(argv[1], O_RDONLY);
    if(fd < 0){
        perror("open");
    }
    while((ret = read(fd, buf, 256)) > 0){
        write(1, buf, ret);
    }
    if(ret < 0){
        perror("read");
    }
    return 0;
}



