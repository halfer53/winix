#include <stdio.h>
#include <sys/syscall.h>
#include <sys/fcntl.h>
#include <string.h>

int main(int argc, char *argv[]){
    if(argc < 2)
        return -1;
    printf("%s", argv[1]);
    return 0;
}


