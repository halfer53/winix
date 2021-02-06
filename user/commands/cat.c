#include <stdio.h>
#include <sys/syscall.h>
#include <sys/fcntl.h>


int main(int argc, char *argv[]){
    int fd, ret;
    char buf[256]; 

    if(argc < 2){
        // fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
        fd = STDIN_FILENO;
    }else{
        fd = open(argv[1], O_RDONLY);
        if(fd < 0){
            perror("open");
            return 1;
        }
    }
    
    while((ret = read(fd, buf, 256)) > 0){
        write(STDOUT_FILENO, buf, ret);
    }
    if(ret < 0){
        perror("read");
    }
    return 0;
}



