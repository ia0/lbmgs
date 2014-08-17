#include <game.h>
#include <chat.h>

struct game *games[] = {
	&chat_game,
};

size_t gameslen = sizeof(games)/sizeof(games[0]);
