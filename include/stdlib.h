#ifndef _STDLIB_H_
#define _STDLIB_H_ 1

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void* malloc (size_t size);
void free(void *ptr);
void* realloc(void *p,size_t size);
void *calloc(size_t number , size_t size);
int exit(int status);

#endif
