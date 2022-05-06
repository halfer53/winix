#include <stddef.h>

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
