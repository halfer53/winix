#include <lib.h>

void perror(const char *s){
    if(s)
        printf("s",s);
    printf("Errno: %d\n",errno);
}
