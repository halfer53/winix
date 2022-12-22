#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>

long strtol(const char *str, char **endptr, int base) {
    int sign = 1;
    // Initialize result
    long result = 0;
    
    // Skip leading whitespace
    while (isspace(*str)) str++;

    // Determine sign
    if (*str == '+') {
        str++;
    } else if (*str == '-') {
        sign = -1;
        str++;
    }

    // Determine base if not specified
    if (base == 0) {
        if (*str == '0') {
            if (tolower(*(str + 1)) == 'x') {
                base = 16;
                str += 2;
            } else {
                base = 8;
                str++;
            }
        } else {
            base = 10;
        }
    }

    // Check for invalid base
    if (base < 2 || base > 36) {
        if (endptr) *endptr = (char *)str;
        return 0;
    }

    

    // Process digits
    while (isxdigit(*str)) {
        int digit;
        if (isdigit(*str)) {
            digit = *str - '0';
        } else {
            digit = toupper(*str) - 'A' + 10;
        }
        if (digit >= base) break;
        if (result > (LONG_MAX - digit) / base) {
            // Overflow
            result = sign == 1 ? LONG_MAX : LONG_MIN;
            errno = ERANGE;
        } else {
            result = result * base + digit;
        }
        str++;
    }

    // Set endptr to point to the first character that is not part of the number
    if (endptr) *endptr = (char *)str;

    return sign * result;
}
