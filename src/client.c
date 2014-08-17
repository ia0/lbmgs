#include <errno.h>
#include <string.h>

#include <common.h>
#include <client.h>

int __attribute__((format(printf, 1, 2)))
cprintf(char *fmt, ...);

void
cflush(void);

void
client_init(struct client_state *state)
{
	cprintf("> ");
	cflush();
	memset(state, 0, sizeof(*state));
	return;
}

int
client_process(struct client_state *state, char *line)
{
	(void)state;
	if (!strcmp(line, "list")) {
		cprintf("list: '%s'\n", &line[4]);
	} else if (!strcmp(line, "help")) {
		cprintf("Available commands are:\n");
		cprintf("  help    print this help\n");
		cprintf("  list    list games\n");
		cprintf("  quit    quit the server\n");
	} else if (!strcmp(line, "quit")) {
		return -1;
	} else {
		cprintf("Invalid command '%s' (try help).\n", line);
	}
	cprintf("> ");
	cflush();
	return 0;
}

void
client_clean(__attribute__((unused)) struct client_state *state)
{
	return;
}
