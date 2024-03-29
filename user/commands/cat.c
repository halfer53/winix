#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#define BUFFER_SIZ  512

int main(int argc, char *argv[]){
    int fd, ret;
    char buf[BUFFER_SIZ]; 
    struct stat statbuf;
    char *file = argv[1];

    if(argc < 2){
        // fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
        fd = STDIN_FILENO;
    }else{
        ret = stat(file, &statbuf);
        if(ret){
            perror("open");
            return 1;
        }
        if(S_ISDIR(statbuf.st_mode)){
            fprintf(stderr, "%s is a directory\n", file);
            return 1;
        }
        fd = open(file, O_RDONLY);
        if(fd < 0){
            perror("open");
            return 1;
        }
    }
    
    while((ret = read(fd, buf, BUFFER_SIZ * sizeof(char))) > 0){
        write(STDOUT_FILENO, buf, ret);
    }
    if(ret < 0){
        perror("read");
    }
    return 0;
}



