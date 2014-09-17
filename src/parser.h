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

#ifndef PARSER_H
#define PARSER_H

/*
 * parsef(): sscanf-like
 * @input: string to make progress on
 * @fmt:   format
 * @...:   outputs
 *
 * The format is a sequence of characters escaped with '%'. A
 * non-escaped character has to match exactly. An escaped character
 * match according to its next character:
 *   %     matches a '%' (no output)
 *   +     matches a non-empty sequence of white-spaces (no output)
 *   *     matches a sequence of (or no) white-spaces (no output)
 *   .     matches the null-terminator (no output)
 *   d     matches an int (one output of type int*)
 *
 * Return: 1 for success and 0 for failure (in which case @input is
 * not modified)
 */
int
parsef(char **input, char *fmt, ...);

#endif /* parser.h */
