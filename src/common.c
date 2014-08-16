#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void __attribute__((format(printf, 1, 2), noreturn))
die(char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}
