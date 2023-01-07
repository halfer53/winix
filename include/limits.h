#ifndef _LIMITS_H_
#define _LIMITS_H_

#include <sys/limits.h>

/* We don't have #include_next.
   Define ANSI <limits.h> for standard 32-bit words.  */

/* These assume 8-bit `char's, 16-bit `short int's,
   and 32-bit `int's and `long int's.  */



/* Minimum and maximum values a `signed char' can hold.  */
#  define SCHAR_MIN        (-128)
#  define SCHAR_MAX        127

/* Maximum value an `unsigned char' can hold.  (Minimum is 0.)  */
#  define UCHAR_MAX        255

/* Minimum and maximum values a `char' can hold.  */
#   define CHAR_MIN        SCHAR_MIN
#   define CHAR_MAX        SCHAR_MAX

/* Minimum and maximum values a `signed short int' can hold.  */
#  define SHRT_MIN        (-32768)
#  define SHRT_MAX        32767

/* Maximum value an `unsigned short int' can hold.  (Minimum is 0.)  */
#  define USHRT_MAX        65535

/* Minimum and maximum values a `signed int' can hold.  */
#  define INT_MAX        2147483647
#  define INT_MIN        (-2147483648)

/* Maximum value an `unsigned int' can hold.  (Minimum is 0.)  */
#  define UINT_MAX        4294967295U

#ifndef PATH_MAX
#define PATH_MAX  WINIX_PATH_MAX
#endif

#ifndef NAME_MAX
#define NAME_MAX  WINIX_NAME_MAX
#endif

#ifdef __wramp__

/* Char is 32 bit in WRAMP since byte size is not even a standard */
#  define CHAR_BIT         32

/* WRAMP architecture defines long as the same length as int */
#define LONG_MIN  INT_MIN
#define LONG_MAX  INT_MAX
#define ULONG_MAX UINT_MAX

#else

/* Number of bits in a `char'.        */
#  define CHAR_BIT         8

#define LONG_MIN (-2147483647L-1)/* minimum value of a long */
#define LONG_MAX  2147483647L	/* maximum value of a long */
#define ULONG_MAX 0xFFFFFFFFL	/* maximum value of an unsigned long */

#endif

#endif
