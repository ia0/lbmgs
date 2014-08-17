#ifndef GAME_H
#define GAME_H

#include <stddef.h>

struct game {
	char *name;
};

extern size_t gameslen;
extern struct game *games[];

#endif /* game.h */
