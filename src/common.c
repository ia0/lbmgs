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

extern FILE *cstream;

int __attribute__((format(printf, 1, 2)))
cprintf(char *fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	assert(cstream != NULL);
	ret = vfprintf(cstream, fmt, ap);
	va_end(ap);
	return ret;
}

void
cflush(void)
{
	assert(cstream != NULL);
	if (fflush(cstream))
		eprintf("cflush: %s\n", strerror(errno));
	return;
}

extern int cfd;

int __attribute__((format(printf, 1, 2)))
gprintf(char *fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	printf("%d: ", cfd);
	ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}

void __attribute__((format(printf, 1, 2)))
geprintf(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	fprintf(stderr, "[1;31m%d: ", cfd);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "[m");
	va_end(ap);
	return;
}
