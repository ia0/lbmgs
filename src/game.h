/*-
 * line-based multiplayer game server
 * Copyright (C) 2014 github at ia0 dot eu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GAME_H
#define GAME_H

struct game {
	char *name;
	void *(*create)(int cid);
	void (*clean)(void *data);
	int (*join)(int cid, void *data);
	int (*leave)(int cid, void *data);
	int (*process)(int cid, void *data, char *line);
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
