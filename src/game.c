#include <game.h>
#include <game/chat.h>

struct game *games[] = {
	&chat_game,
};

size_t gameslen = sizeof(games)/sizeof(games[0]);

struct party parties[PARTY_SLOTS];

int
available_slot(void)
{
	int i;
	for (i = 0; i < PARTY_SLOTS; i++)
		if (parties[i].game == NULL)
			return i;
	return -1;
}
