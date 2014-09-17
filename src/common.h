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
