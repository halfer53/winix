#include <sys/syscall.h>
#include <sys/fcntl.h>


int main(int argc, char *argv[]){
    int fd, ret;
    struct stat statbuf;
    char *path = *++argv;
    if(argc < 2){
        fprintf(stderr, "rm FILE");
        return 1;
    }
    ret = stat(path, &statbuf);
    if(ret){
        perror(path);
        return 1;
    }
    if(!S_ISREG(statbuf.st_mode)){
        fprintf(stderr, "%s is not regular file\n", path);
        return 1;
    }
    ret = unlink(path);
    if(ret){
        perror("rm failed");
    }
    return ret;
}

