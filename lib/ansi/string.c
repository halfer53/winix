#include <lib.h>
#include <limits.h>
#include <ctype.h>



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
size_t strlen(const char *s) {
    const char* bak = s;
    for (; *s; ++s)
                ;
    return s - bak;
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
    const char *s = string;
    while(*s && *s != c)
        s++;
    return (char *)(unsigned long)(*s ? s : NULL);
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
    const char *X = haystack;
    const char *Y = needle;
	while (*X != '\0')
	{
		if ((*X == *Y) && compare(X, Y))
			return (char *)(unsigned long)X;
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
	const char* from = (const char*) src;
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


// unsigned long
// strtoul(nptr, endptr, base)
// 	const char *nptr;
// 	char **endptr;
// 	register int base;
// {
// 	register const char *s = nptr;
// 	register unsigned int acc;
// 	register int c;
// 	register unsigned int cutoff;
// 	register int neg = 0, any, cutlim;

// 	/*
// 	 * See strtol for comments as to the logic used.
// 	 */
// 	do {
// 		c = *s++;
// 	} while (isspace(c));
// 	if (c == '-') {
// 		neg = 1;
// 		c = *s++;
// 	} else if (c == '+')
// 		c = *s++;
// 	if ((base == 0 || base == 16) &&
// 	    c == '0' && (*s == 'x' || *s == 'X')) {
// 		c = s[1];
// 		s += 2;
// 		base = 16;
// 	}
// 	if (base == 0)
// 		base = c == '0' ? 8 : 10;
// 	cutoff = (unsigned int)UINT_MAX / (unsigned int)base;
// 	cutlim = (unsigned int)UINT_MAX % (unsigned int)base;
// 	for (acc = 0, any = 0;; c = *s++) {
// 		if (isdigit(c))
// 			c -= '0';
// 		else if (isalpha(c))
// 			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
// 		else
// 			break;
// 		if (c >= base)
// 			break;
// 		if (any < 0 || acc > cutoff || acc == cutoff && c > cutlim)
// 			any = -1;
// 		else {
// 			any = 1;
// 			acc *= base;
// 			acc += c;
// 		}
// 	}
// 	if (any < 0) {
// 		acc = UINT_MAX;
// 		errno = ERANGE;
// 	} else if (neg){
//         // acc = -acc;
//     }
// 	if (endptr != 0)
// 		*endptr = (char *)(any ? s - 1 : nptr);
// 	return (acc);
// }

