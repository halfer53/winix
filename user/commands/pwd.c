#include <unistd.h>
#include <stdio.h>


int main(int argc, char **argv){
    char tmp[128];
    char *p;

    p = getcwd(tmp, 128);
    if(!p){
        return 1;
    }
    printf("%s\n", p);
    return 0;
}


