#include <lib.h>

void perror(const char *s){
    if(s)
        printf("%s",s);
    printf(" errno is: %d\n",errno);
}
