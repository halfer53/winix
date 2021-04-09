#include <sys/syscall.h>

int main(int argc, char *argv[]){
    int ret;
    if(argc < 3){
        fprintf(stderr, "ln path/to/file path/to/hardlink");
        return 1;
    }
    ret = link(argv[1], argv[2]);
    if(ret)
        perror("");
        
    return ret;
}


