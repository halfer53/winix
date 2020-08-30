#ifndef _STDLIB_H_
#define _STDLIB_H_ 1

#include <stddef.h>
#include <sys/types.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define RAND_MAX       32767    /* largest value generated by rand() */

typedef struct { int quot, rem; } div_t;
typedef struct { long quot, rem; } ldiv_t;

void* malloc (long size);
void free(void *ptr);
void* realloc(void *p,long size);
void *calloc(long number , long size);
char *getenv(const char *name);
int putenv(char *name);
int setenv(const char *name, const char *value, int overwrite);
void srand(unsigned int seed);
int rand(void);

#endif
