#include <lib.h>

int setenv(const char *name, const char *value, int overwrite) {
	char *buf;

	if (!overwrite && getenv(name))
		return 0;

	/* Include sizes plus '=' and trailing null. */
	if (!(buf = malloc(strlen(name) + strlen(value) + 2)))
		return -1;

    strcpy(buf,name);
    strcat(buf,"=");
    strcat(buf,value);
	return putenv(buf);
}
