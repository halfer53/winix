#ifndef _LIMITS_H_
#define _LIMITS_H_

/* We don't have #include_next.
   Define ANSI <limits.h> for standard 32-bit words.  */

/* These assume 8-bit `char's, 16-bit `short int's,
   and 32-bit `int's and `long int's.  */

/* Number of bits in a `char'.        */
#  define CHAR_BIT        8

/* Minimum and maximum values a `signed char' can hold.  */
#  define SCHAR_MIN        (-128)
#  define SCHAR_MAX        127

/* Maximum value an `unsigned char' can hold.  (Minimum is 0.)  */
#  define UCHAR_MAX        255

/* Minimum and maximum values a `char' can hold.  */
#  ifdef __CHAR_UNSIGNED__
#   define CHAR_MIN        0
#   define CHAR_MAX        UCHAR_MAX
#  else
#   define CHAR_MIN        SCHAR_MIN
#   define CHAR_MAX        SCHAR_MAX
#  endif

/* Minimum and maximum values a `signed short int' can hold.  */
#  define SHRT_MIN        (-32768)
#  define SHRT_MAX        32767

/* Maximum value an `unsigned short int' can hold.  (Minimum is 0.)  */
#  define USHRT_MAX        65535

/* Minimum and maximum values a `signed int' can hold.  */
#  define INT_MIN        (-INT_MAX - 1)
#  define INT_MAX        2147483647

/* Maximum value an `unsigned int' can hold.  (Minimum is 0.)  */
#  define UINT_MAX        4294967295U

#endif
