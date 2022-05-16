#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    int ret;
    if(argc < 3){
        fprintf(stderr, "usage: ln [path/to/file] [path/to/hardlink]\n");
        return 1;
    }
    ret = link(argv[1], argv[2]);
    if(ret)
        perror("");
        
    return ret;
}


