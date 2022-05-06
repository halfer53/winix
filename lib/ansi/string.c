#include <lib.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>





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

