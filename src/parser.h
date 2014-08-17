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
