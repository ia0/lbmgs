#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>

#include <common.h>

static int
parse_int(char **input, int *output)
{
	char *end;
	long int res;
	res = strtol(*input, &end, 10);
	if (*input == end)
		return 0;
	if (INT_MIN <= res && res <= INT_MAX) {
		*output = res;
		*input = end;
		return 1;
	}
	return 0;
}

int
parsef(char **input, char *fmt, ...)
{
	va_list ap;
	char *cur;
	int *d;
	cur = *input;
	va_start(ap, fmt);
	for(; *fmt; fmt++) {
		if (*fmt != '%') {
			if (*cur != *fmt)
				goto err;
			cur++;
			continue;
		}
		fmt++;
		switch(*fmt) {
		case '%':
			if (*cur != '%')
				goto err;
			cur++;
			break;
		case '+':
			if (!isspace((unsigned char)*cur))
				goto err;
		case '*':
			while (isspace((unsigned char)*cur))
				cur++;
			break;
		case '.':
			if (*cur != '\0')
				goto err;
			cur++;
			break;
		case 'd':
			d = va_arg(ap, int *);
			if (!parse_int(&cur, d))
				goto err;
			break;
		default:
			eprintf("parsef: invalid escaped char '%c'\n", *fmt);
			goto err;
		}
	}
	va_end(ap);
	*input = cur;
	return 1;

err:
	va_end(ap);
	return 0;
}
