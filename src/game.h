#ifndef GAME_H
#define GAME_H

#include <stddef.h>

struct game {
	char *name;
	void *(*init)(int id);
	void (*process)(void *data, char *line);
};

extern size_t gameslen;
extern struct game *games[];

struct party {
	struct game *game; /* static */
	void *data; /* allocated */
};

#define PARTY_SLOTS 10
extern struct party parties[PARTY_SLOTS];

/*
 * available_slot(): find an available slot
 *
 * Return: -1 if no slot is available otherwise the slot index
 */
int
available_slot(void);

#endif /* game.h */
