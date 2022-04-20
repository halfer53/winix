#ifndef _CTYPE_H_
#define _CTYPE_H_ 1

#define isdigit(c)	((unsigned) ((c)-'0') < 10)
#define islower(c)	((unsigned) ((c)-'a') < 26)
#define isupper(c)	((unsigned) ((c)-'A') < 26)
#define isprint(c)	((unsigned) ((c)-' ') < 95)
#define isascii(c)	((unsigned) (c) < 128)
#define isspace(c)    (c==' ')
#define isalpha(c)  (isupper(c) || islower(c))
#endif
