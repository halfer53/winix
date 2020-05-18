#include <stdio.h>
#include <sys/syscall.h>
#include <sys/fcntl.h>


int main(int argc, char *argv[]){
    if(argc < 2)
        return -1;
    printf("%s\n", argv[1]);
    return 0;
}



