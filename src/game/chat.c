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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <game/chat.h>

#include <common.h>
#include <client.h>

#define ID_SLOTS 10
struct chat {
	int ids[ID_SLOTS];
};

static void *
chat_create(int cid)
{
	struct chat *chat;
	int i;

	chat = malloc(sizeof(*chat));
	chat->ids[0] = cid;
	for (i = 1; i < ID_SLOTS; i++)
		chat->ids[i] = -1;

	cprintf(cid, "Start chat.\n");

	return chat;
}

static int
chat_join(int cid, void *data)
{
	struct chat *chat = data;
	int i;

	assert(valid_cid(cid));
	assert(chat != NULL);

	for (i = 0; i < ID_SLOTS; i++)
		if (chat->ids[i] == -1)
			break;
	if (i == ID_SLOTS) {
		cprintf(cid, "No slots available.\n");
		return 0;
	}

	chat->ids[i] = cid;

	cprintf(cid, "Start chat.\n");

	for (i = 0; i < ID_SLOTS; i++) {
		int pid = chat->ids[i];
		if (pid != -1 && pid != cid) {
			cprintf(pid, "\n%d has joined.\n", cid);
			cprompt(pid);
		}
	}

	return 1;
}

static void
chat_clean(void *data)
{
	assert(data != NULL);
	free(data);
	return;
}

static int
chat_leave(int cid, void *data)
{
	struct chat *chat = data;
	int i, last;

	assert(valid_cid(cid));
	assert(chat != NULL);

	for (i = 0; i < ID_SLOTS; i++)
		if (chat->ids[i] == cid)
			break;
	if (i == ID_SLOTS) {
		geprintf(cid, "could not find chat slot\n");
		return 0;
	}

	chat->ids[i] = -1;

	cprintf(cid, "Ending chat.\n");

	for (last = 1, i = 0; i < ID_SLOTS; i++) {
		int pid = chat->ids[i];
		if (pid != -1) {
			assert(pid != cid);
			last = 0;
			cprintf(pid, "\n%d has left.\n", cid);
			cprompt(pid);
		}
	}
	if (last)
		return -1;

	return 0;
}

static int
chat_process(int cid, void *data, char *line)
{
	struct chat *chat = data;
	int i;

	assert(chat != NULL);

	if (!*line)
		return -1;

	for (i = 0; i < ID_SLOTS; i++) {
		int pid = chat->ids[i];
		if (pid != -1) {
			if (pid != cid) {
				cprintf(pid, "\n%d: %s\n", cid, line);
				cprompt(pid);
			} else {
				cprintf(cid, "%d) %s\n", cid, line);
			}
		}
	}

	return 0;
}

struct game chat_game = {
	.name = "chat",
	.create = chat_create,
	.join = chat_join,
	.leave = chat_leave,
	.process = chat_process,
	.clean = chat_clean,
};
