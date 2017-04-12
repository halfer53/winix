#include <stdio.h>      /* printf */
#include <string.h>     /* strcat */
#include <stdlib.h>     /* strtol */
int main(int argc, char const *argv[]) {
  int n = 0;
  int i =0;
  int a = 0;
  int wordslength = 2049;
  wordslength = wordslength % 1024 == 0 ? wordslength : wordslength + 1024;
  wordslength = wordslength < 1024 ? 1024 : ((wordslength / 1024) * 1024);
  printf("wordslength %d\n",wordslength );
  wordslength = 2047;
  wordslength = wordslength % 1024 == 0 ? wordslength : wordslength + 1;
  wordslength = wordslength < 1024 ? 1024 : ((wordslength / 1024) * 1024);
  printf("wordslength %d\n",wordslength );
  wordslength = 2048;
  wordslength = wordslength % 1024 == 0 ? wordslength : wordslength + 1;
  wordslength = wordslength < 1024 ? 1024 : ((wordslength / 1024) * 1024);
  printf("wordslength %d\n",wordslength );


  n = 2048 / 1024;
  for (i = n; i < n + wordslength/1024 +1; i++) {
    a |= (0x80000000 >> i);
  }
  printBits(sizeof(a),&a);

  n = 0x3000 / 1024;
  printf("%d\n",n );
  a |= (0x80000000 >> n);
  a |= (0x80000000 >> n+1);

  printBits(sizeof(a),&a);
  a = 0;
  for ( i = 0; i < n; i++) {
    a |= (0x80000000 >> i);
  }
  printBits(sizeof(a),&a);
  return 0;
}

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}
