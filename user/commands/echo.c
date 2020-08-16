#include <stdio.h>
#include <sys/syscall.h>
#include <sys/fcntl.h>
#include <string.h>

char test[] = {0x1b, 0};
char cls[] = {0x1b, 0x5b, 0x31, 0x3b, 0x31, 0x48, 0};
char cls2[] = {0x1b, 0x5b, 0x32, 0x4a, 0};

int main(int argc, char *argv[]){
    printf("test %s", test);
    printf("%s", cls);
    printf("%s", cls2);
    if(argc < 2)
        return -1;
    printf("%s", argv[1]);
    return 0;
}



