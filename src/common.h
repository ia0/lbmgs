#ifndef COMMON_H
#define COMMON_H

void __attribute__((format(printf, 1, 2), noreturn))
die(char *msg, ...);

#define die_(fmt, ...)						\
	die("%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__)

void __attribute__((format(printf, 1, 2)))
eprintf(char *fmt, ...);

int __attribute__((format(printf, 1, 2)))
cprintf(char *fmt, ...);

void
cflush(void);

int __attribute__((format(printf, 1, 2)))
gprintf(char *fmt, ...);

int __attribute__((format(printf, 1, 2)))
geprintf(char *fmt, ...);

#endif /* common.h */
