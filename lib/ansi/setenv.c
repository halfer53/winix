#include <lib.h>

int setenv(const char *name, const char *value, int overwrite) {
	char *buf;
	int len;

	if (!overwrite && getenv(name))
		return 0;
	len = strlen(name) + strlen(value) + 2;
	/* Include sizes plus '=' and trailing null. */
	if (!(buf = malloc(len)))
		return -1;

    strlcpy(buf,name, len);
    strlcat(buf,"=", len);
    strlcat(buf,value, len);
	return putenv(buf);
}
