#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <common.h>
#include <client.h>
#include <parser.h>
#include <game.h>

extern FILE *streams[SLOTS];

int __attribute__((format(printf, 2, 3)))
cprintf(int cid, char *fmt, ...)
{
	int ret;
	va_list ap;

	assert(valid_cid(cid));
	assert(streams[cid] != NULL);

	va_start(ap, fmt);
	ret = vfprintf(streams[cid], fmt, ap);
	va_end(ap);

	return ret;
}

void
cflush(int cid)
{
	assert(valid_cid(cid));
	assert(streams[cid] != NULL);

	if (fflush(streams[cid]))
		geprintf(cid, "cflush: %s\n", strerror(errno));
	return;
}

void
client_init(int cid)
{
	assert(valid_cid(cid));

	cprintf(cid, "Type 'list' to list games and parties or 'help' for help.\n");
	cprintf(cid, "> ");
	cflush(cid);

	assert(clients[cid].party = -1);

	return;
}

static void
cmd_list(int cid)
{
	int i;

	assert(valid_cid(cid));

	cprintf(cid, "Available games:\n");

	for (i = 0; i < gameslen; i++)
		cprintf(cid, "  %d: %s\n", i, games[i]->name);

	cprintf(cid, "Type 'create <n>' to create a party of game <n>.\n");

	return;
}

static void
cmd_create(int cid, int g)
{
	int p;

	assert(valid_cid(cid));

	p = available_party();
	if (p < 0) {
		cprintf(cid, "No slot available.\n");
		return;
	}

	clients[cid].party = p;
	parties[p].game = games[g];
	assert(games[g]->init != NULL);
	parties[p].data = games[g]->init(cid);

	return;
}

int
client_process(int cid, char *line)
{
	int arg;
	int p;

	assert(valid_cid(cid));

	p = clients[cid].party;

	if (p >= 0) {
		assert(valid_party(p));
		assert(parties[p].game->process != NULL);
		parties[p].game->process(cid, parties[p].data, line);
		goto end;
	}

	parsef(&line, "%*");

	if (parsef(&line, "list%*")) {
		if (!parsef(&line, "%."))
			goto invalid;
		cmd_list(cid);
	} else if (parsef(&line, "join%+")) {
		if (!parsef(&line, "%d%*%.", &arg))
			goto invalid;
		cprintf(cid, "todo join %d\n", arg);
	} else if (parsef(&line, "create%+")) {
		if (!parsef(&line, "%d%*%.", &arg))
			goto invalid;
		cmd_create(cid, arg);
	} else if (parsef(&line, "quit%*")) {
		if (!parsef(&line, "%."))
			goto invalid;
		return -1;
	} else if (parsef(&line, "help%*")) {
		if (!parsef(&line, "%."))
			goto invalid;
		cprintf(cid, "Available commands are:\n");
		cprintf(cid, "  list    list games and parties\n");
		cprintf(cid, "  join    join a party\n");
		cprintf(cid, "  create  create a party\n");
		cprintf(cid, "  quit    quit the server\n");
		cprintf(cid, "  help    print this help\n");
	} else {
		goto invalid;
	}
	goto end;

invalid:
	cprintf(cid, "Invalid trailing '%s'.\n", line);
end:
	cprintf(cid, "> ");
	cflush(cid);
	return 0;
}

void
client_clean(int cid)
{
	clients[cid].party = -1;
	return;
}
