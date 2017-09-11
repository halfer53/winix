
#include <stdio.h>
#include <unistd.h>

int main(){
    char *str[] = {"hello world", "you are", NULL};
printf("%s\n",str[0]);
printf("%s\n",str[1]);
    return 0;
}

int foo(){

    return 100;
}
