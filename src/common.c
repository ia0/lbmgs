#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

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
}
