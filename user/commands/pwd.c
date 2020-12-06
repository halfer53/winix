#include <sys/syscall.h>


int main(int argc, char **argv){
    char tmp[128];
    char *p;
    int ret;

    p = getcwd(tmp, 128);
    if(!p){
        return 1;
    }
    printf("%s", p);
    return 0;
}


