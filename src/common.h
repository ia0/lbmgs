#ifndef COMMON_H
#define COMMON_H

void __attribute__((format(printf, 1, 2), noreturn))
die(char *msg, ...);

#define die_(fmt, ...)						\
	die("%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__)

void __attribute__((format(printf, 1, 2)))
eprintf(char *fmt, ...);

int __attribute__((format(printf, 2, 3)))
gprintf(int cid, char *fmt, ...);

int __attribute__((format(printf, 2, 3)))
geprintf(int cid, char *fmt, ...);

#endif /* common.h */
