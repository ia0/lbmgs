#ifndef COMMON_H
#define COMMON_H

void __attribute__((format(printf, 1, 2), noreturn))
die(char *msg, ...);

#define die_(fmt, ...)						\
	die("%s:%d: " fmt, __FILE__, __LINE__, __VA_ARGS__)

#endif /* common.h */
