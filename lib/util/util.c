#include <util.h>

const char *size_t_to_binary(unsigned int x)
{
    static char b[32];
    int z;
    b[0] = '\0';

    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

int hex2int(char *a, int len)
{
    int i;
    int val = 0;

    for(i=0;i<len;i++){
        if(a[i] <= 57)
             val += (a[i]-48)*(1<<(4*(len-1-i)));
        else
             val += (a[i]-55)*(1<<(4*(len-1-i)));
    }

    return val;
}
