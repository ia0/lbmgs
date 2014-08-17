#ifndef GAME_H
#define GAME_H

struct game {
	char *name;
	void *(*create)(int cid);
	int (*join)(int cid, void *data);
	int (*leave)(int cid, void *data);
	int (*process)(int cid, void *data, char *line);
	void (*clean)(void *data);
};

extern int gameslen;
extern struct game *games[];

static inline int
valid_game(int g)
{
	return 0 <= g && g < gameslen;
}

struct party {
	struct game *game; /* NULL when no game running */
	void *data; /* allocated (NULL when game is NULL) */
};

#define PARTY_SLOTS 10
extern struct party parties[PARTY_SLOTS];

static inline int
valid_party(int p)
{
	return 0 <= p && p < PARTY_SLOTS;
}

/*
 * available_party(): find an available party slot
 *
 * Return: -1 if no slot is available otherwise the slot index
 */
int
available_party(void);

#endif /* game.h */
