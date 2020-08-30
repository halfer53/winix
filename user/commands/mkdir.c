#include <sys/syscall.h>
#include <sys/fcntl.h>

int main(int argc, char *argv[]){
    int fd;
    if(argc < 2 ){
        fprintf(stderr, "mising file operand\n");
        return 1;
    }
    fd = mkdir(argv[1], 0x755);
    if(fd < 0){
        perror("mkdir");
        return 1;
    }
    close(fd);
    return 0;
}


