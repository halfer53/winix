#include <util.h>


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
