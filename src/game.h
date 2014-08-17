#ifndef GAME_H
#define GAME_H

struct game {
	char *name;
	void *(*init)(int cid);
	void (*process)(int cid, void *data, char *line);
};

extern int gameslen;
extern struct game *games[];

#ifndef NDEBUG
static inline int
valid_game(int g)
{
	return 0 <= g && g < gameslen;
}
#endif

struct party {
	struct game *game; /* static */
	void *data; /* allocated */
};

#define PARTY_SLOTS 10
extern struct party parties[PARTY_SLOTS];

#ifndef NDEBUG
static inline int
valid_party(int p)
{
	return 0 <= p && p < PARTY_SLOTS;
}
#endif

/*
 * available_party(): find an available party slot
 *
 * Return: -1 if no slot is available otherwise the slot index
 */
int
available_party(void);

#endif /* game.h */
