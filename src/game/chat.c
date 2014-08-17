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
			cflush(pid);
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
			cflush(pid);
		}
	}
	if (last) {
		chat_clean(data);
		return -1;
	}

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
				cflush(pid);
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
