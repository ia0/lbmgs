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
