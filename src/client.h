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

#ifndef CLIENT_H
#define CLIENT_H

struct client {
	int party; /* -1 for lobby */
};

#define SLOTS 20
extern struct client clients[SLOTS];

static inline int
valid_cid(int cid)
{
	return 0 <= cid && cid < SLOTS;
}

/* cprintf(): print to a client */
int __attribute__((format(printf, 2, 3)))
cprintf(int cid, char *fmt, ...);

/* cflush(): flush the stream of a client */
void
cflush(int cid);

/* cprompt(): shows the prompt for a client */
void
cprompt(int cid);

/* client_init(): initialize the client */
void
client_init(int cid);

/* client_process(): process a line from the client */
int /* success == 0, failure == -1 */
client_process(int cid, char *line);

/* client_leave(): meak the client leave its party */
void
client_leave(int cid);

/* client_clean(): cleans the client state */
void
client_clean(int cid);

#endif /* client.h */
