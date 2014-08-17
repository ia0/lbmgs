#include <assert.h>
#include <errno.h>
#include <string.h>

#include <common.h>
#include <client.h>
#include <parser.h>
#include <game.h>

void
client_init(struct client_state *state, int id)
{
	cprintf("Type 'list' to list games and parties or 'help' for help.\n");
	cprintf("> ");
	cflush();
	state->id = id;
	state->party = -1;
	return;
}

static void
cmd_list(void)
{
	size_t i;

	cprintf("Available games:\n");

	for (i = 0; i < gameslen; i++)
		cprintf("  %zu: %s\n", i, games[i]->name);

	cprintf("Type 'create <n>' to create a party of game <n>.\n");

	return;
}

static void
cmd_create(struct client_state *state, int g)
{
	int p;
	struct game *game;
	struct party *party;

	p = available_slot();
	if (p < 0) {
		cprintf("No slot available.\n");
		return;
	}

	game = games[g];
	party = &parties[p];
	state->party = p;
	party->game = game;
	assert(game->init != NULL);
	party->data = game->init(state->id);

	return;
}

int
client_process(struct client_state *state, char *line)
{
	int arg;

	if (state->party >= 0) {
		struct party *party;
		assert(state->party < PARTY_SLOTS);
		party = &parties[state->party];
		assert(party->game->process);
		party->game->process(party->data, line);
		goto end;
	}

	parsef(&line, "%*");

	if (parsef(&line, "list%*")) {
		if (!parsef(&line, "%."))
			goto invalid;
		cmd_list();
	} else if (parsef(&line, "join%+")) {
		if (!parsef(&line, "%d%*%.", &arg))
			goto invalid;
		cprintf("todo join %d\n", arg);
	} else if (parsef(&line, "create%+")) {
		if (!parsef(&line, "%d%*%.", &arg))
			goto invalid;
		cmd_create(state, arg);
	} else if (parsef(&line, "quit%*")) {
		if (!parsef(&line, "%."))
			goto invalid;
		return -1;
	} else if (parsef(&line, "help%*")) {
		if (!parsef(&line, "%."))
			goto invalid;
		cprintf("Available commands are:\n");
		cprintf("  list    list games and parties\n");
		cprintf("  join    join a party\n");
		cprintf("  create  create a party\n");
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
