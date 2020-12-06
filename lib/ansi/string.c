#include <lib.h>
void *memcpy(void *s1, const void *s2, size_t n)
{
    char *p1 = s1;
    const char *p2 = s2;

    if (n) {
        n++;
        while (--n > 0) {
            *p1++ = *p2++;
        }
    }
    return s1;
}

/**
 * Compares two strings.
 **/
int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) {
            break;
        }
        s1++;
        s2++;
    }

    return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t size) {
    while (*s1 && *s2 && size--) {
        if (*s1 != *s2) {
            break;
        }
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

/**
 * Calculates the length of a string.
 **/
int strlen(const char *s) {
    const char* bak = s;
    for (; *s; ++s)
                ;
    return s - bak;
}

/**
 * copy the src into the destination string, the destination should be large enough to hold the src
 **/
char *strcpy(char *dest, const char *src) {
    char *saved = dest;
    while (*dest++ = *src++);
    *dest = 0;
    return saved;
}

char *strncpy(char *dest, const char *src, size_t n){
    char *saved = dest;
    while (*src && n--) {
        *dest++ = *src++;
    }
    *dest = 0;
    return saved;
}

char *strcat(char *dest, const char *src) {
    char* saved = dest;
    while(*dest++);
    dest--;
    while(*dest++ = *src++);
    *dest = '\0';
    return saved;
}

void *memset(void *dst, int c, size_t n)
{
    if (n) {
         char *d = dst;
 
         do {
             *d++ = c;
         } while (--n);
     }
     return dst;
}

/**
 * return the length of substring of the original string, starting at start_index, and has specified length
 **/
int substring(char* buffer, char* original, int start_index, int length) {
    int i = 0;
    int count = 0;
    for (i = start_index; i < length + start_index; i++)
    {
        buffer[count] = original[i];
        count++;
    }
    buffer[count] = '\0';
    return count;
}


char *index(const char *string, int c){
    char *s = (char *)string;
    while(*s && *s != c)
        s++;
    return *s ? s : NULL;
}

#include <stdio.h>

// returns true if X and Y are same
int compare(const char *X, const char *Y)
{
	while (*X && *Y)
	{
		if (*X != *Y)
			return 0;

		X++;
		Y++;
	}

	return (*Y == '\0');
}

// Function to implement strstr() function
char* strstr(const char* haystack, const char* needle)
{
    char *X = (char *)haystack;
    char *Y = (char *)needle;
	while (*X != '\0')
	{
		if ((*X == *Y) && compare(X, Y))
			return X;
		X++;
	}

	return NULL;
}

size_t strcspn(const char *s1, const char *s2)
{
    size_t ret=0;
    while(*s1)
        if(strchr(s2,*s1))
            return ret;
        else
            s1++,ret++;
    return ret;
}

size_t strspn(const char *s1, const char *s2)
{
    size_t ret=0;
    while(*s1 && strchr(s2,*s1++))
        ret++;
    return ret;    
}

void *memmove(void *dest, const void *src, size_t n)
{
	char* from = (char*) src;
	char* to = (char*) dest;

	if (from == to || n == 0)
		return dest;
	if (to > from && to-from < (int)n) {
		/* to overlaps with from */
		/*  <from......>         */
		/*         <to........>  */
		/* copy in reverse, to avoid overwriting from */
		int i;
		for(i=n-1; i>=0; i--)
			to[i] = from[i];
		return dest;
	}
	if (from > to && from-to < (int)n) {
		/* to overlaps with from */
		/*        <from......>   */
		/*  <to........>         */
		/* copy forwards, to avoid overwriting from */
		size_t i;
		for(i=0; i<n; i++)
			to[i] = from[i];
		return dest;
	}
	memcpy(dest, src, n);
	return dest;
}

