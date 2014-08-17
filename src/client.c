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
cprompt(int cid)
{
	int p;

	assert(valid_cid(cid));

	p = clients[cid].party;

	if (p == -1) {
		cprintf(cid, "%d> ", cid);
		cflush(cid);
		return;
	}

	assert(valid_party(p));
	assert(parties[p].game != NULL);

	cprintf(cid, "%d %s> ", cid, parties[p].game->name);
	cflush(cid);
	return;
}

void
client_init(int cid)
{
	assert(valid_cid(cid));

	cprintf(cid, "Type 'list' to list games and parties or 'help' for help.\n");
	cprompt(cid);

	assert(clients[cid].party = -1);

	return;
}

static void
cmd_list(int cid)
{
	int i;

	assert(valid_cid(cid));

	cprintf(cid, "Available parties (to join):\n");

	for (i = 0; i < PARTY_SLOTS; i++)
		if (parties[i].game != NULL)
			cprintf(cid, "%4d:  %s\n", i, parties[i].game->name);

	cprintf(cid, "Available games (to create):\n");

	for (i = 0; i < gameslen; i++)
		cprintf(cid, "%4d:  %s\n", i, games[i]->name);

	return;
}

static void
cmd_join(int cid, int p)
{
	assert(valid_cid(cid));

	if (!valid_party(p) || parties[p].game == NULL) {
		cprintf(cid, "Invalid party.\n");
		return;
	}

	assert(parties[p].game->join != NULL);

	if (parties[p].game->join(cid, parties[p].data))
		clients[cid].party = p;
}

static void
cmd_create(int cid, int g)
{
	int p;

	assert(valid_cid(cid));

	if (!valid_game(g)) {
		cprintf(cid, "Not a valid game.\n");
		return;
	}

	assert(games[g]->create != NULL);

	p = available_party();
	if (p < 0) {
		cprintf(cid, "No slot available.\n");
		return;
	}

	clients[cid].party = p;
	parties[p].game = games[g];
	parties[p].data = games[g]->create(cid);

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
		if (parties[p].game->process(cid, parties[p].data, line) < 0)
			client_clean(cid);
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
		cmd_join(cid, arg);
	} else if (parsef(&line, "create%+")) {
		if (!parsef(&line, "%d%*%.", &arg))
			goto invalid;
		cmd_create(cid, arg);
	} else if (parsef(&line, "quit%*")) {
		if (!parsef(&line, "%."))
			goto invalid;
		return -1;
	} else if (parsef(&line, "%.")) {
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
	cprompt(cid);
	return 0;
}

void
client_clean(int cid)
{
	int p;

	assert(valid_cid(cid));

	p = clients[cid].party;

	if (p == -1)
		return;

	assert(valid_party(p));
	assert(parties[p].game->leave != NULL);
	if (parties[p].game->leave(cid, parties[p].data)) {
		parties[p].game = NULL;
		parties[p].data = NULL;
	}
	clients[cid].party = -1;

	return;
}
