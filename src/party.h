#ifndef PARTY_H
#define PARTY_H

#include <game.h>

struct party {
	struct game *game;
	void *data;
};

#define PARTY_SLOTS 10
extern struct party parties[PARTY_SLOTS];

#endif /* party.h */
