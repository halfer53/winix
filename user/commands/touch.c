#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    int fd;
    if(argc < 2 ){
        fprintf(stderr, "mising file operand\n");
        return 1;
    }
    fd = open(argv[1], O_CREAT | O_RDWR, 0755 );
    close(fd);
    return 0;
}




