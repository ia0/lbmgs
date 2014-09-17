/*-
 * line-based multiplayer game server
 * Copyright (C) 2014 github at ia0 dot eu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
