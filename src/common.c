#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void __attribute__((format(printf, 1, 2), noreturn))
die(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "[1;41m");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "[m");
	va_end(ap);
	exit(EXIT_FAILURE);
}

void __attribute__((format(printf, 1, 2)))
eprintf(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "[1;31m");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "[m");
	va_end(ap);
	return;
}

int __attribute__((format(printf, 2, 3)))
gprintf(int cid, char *fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	printf("%d: ", cid);
	ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}

void __attribute__((format(printf, 2, 3)))
geprintf(int cid, char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "[1;31m%d: ", cid);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "[m");
	va_end(ap);
	return;
}
