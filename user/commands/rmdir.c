#include <unistd.h>

int main(int argc, char *argv[]){
    int ret;
    char *path = *++argv;
    if(argc < 2){
        fprintf(stderr, "usage: rmdir [DIRECTORY]\n");
        return 1;
    }
    ret = rmdir(path);
    if(ret){
        perror("rmdir failed");
    }
    return ret;
}
