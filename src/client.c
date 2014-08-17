#include <errno.h>
#include <string.h>

#include <common.h>
#include <client.h>
#include <parser.h>

int __attribute__((format(printf, 1, 2)))
cprintf(char *fmt, ...);

void
cflush(void);

void
client_init(struct client_state *state)
{
	cprintf("Type 'list' to list games or 'help' for help.\n");
	cprintf("> ");
	cflush();
	memset(state, 0, sizeof(*state));
	return;
}

int
client_process(struct client_state *state, char *line)
{
	int arg;
	(void)state;
	parsef(&line, "%*");
	if (parsef(&line, "list%*")) {
		if (!parsef(&line, "%."))
			goto invalid;
		cprintf("todo list\n");
	} else if (parsef(&line, "join%+")) {
		if (!parsef(&line, "%d%*%.", &arg))
			goto invalid;
		cprintf("todo join %d\n", arg);
	} else if (parsef(&line, "create%+")) {
		if (!parsef(&line, "%d%*%.", &arg))
			goto invalid;
		cprintf("todo create %d\n", arg);
	} else if (parsef(&line, "quit%*")) {
		if (!parsef(&line, "%."))
			goto invalid;
		return -1;
	} else if (parsef(&line, "help%*")) {
		if (!parsef(&line, "%."))
			goto invalid;
		cprintf("Available commands are:\n");
		cprintf("  list    list games\n");
		cprintf("  join    join a game\n");
		cprintf("  create  create a game\n");
		cprintf("  quit    quit the server\n");
		cprintf("  help    print this help\n");
	} else {
		goto invalid;
	}
	goto end;

invalid:
	cprintf("Invalid trailing '%s'.\n", line);
end:
	cprintf("> ");
	cflush();
	return 0;
}

void
client_clean(__attribute__((unused)) struct client_state *state)
{
	return;
}
