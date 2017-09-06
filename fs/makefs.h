#ifndef _MAKEFS_H_
#define _MAKEFS_H_ 1

//16 MB
// #define totalsize (16*1024*1024)

#define totalsize   (16*1024*1024)
extern char disk[totalsize+1];

int makefs();
#endif