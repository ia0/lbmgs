#include <stddef.h>

#include <game.h>
#include <game/chat.h>
#include <game/chess.h>

struct game *games[] = {
	&chat_game,
	&chess_game,
};

int gameslen = sizeof(games)/sizeof(games[0]);

struct party parties[PARTY_SLOTS];

int
available_party(void)
{
	int i;
	for (i = 0; i < PARTY_SLOTS; i++)
		if (parties[i].game == NULL)
			return i;
	return -1;
}
