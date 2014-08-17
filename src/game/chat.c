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
chat_init(int cid)
{
	struct chat *chat;
	int i;

	chat = malloc(sizeof(*chat));
	chat->ids[0] = cid;
	for (i = 1; i < ID_SLOTS; i++)
		chat->ids[i] = -1;

	cprintf(cid, "Start chat.\n");

	return NULL;
}

static void
chat_process(int cid, void *data, char *line)
{
	struct chat *chat = data;
	int i;

	for (i = 0; i < ID_SLOTS; i++) {
		int pid = chat->ids[i];
		if (pid != -1) {
			if (pid != cid) {
				cprintf(pid, "%s\n", line);
				cflush(pid);
			} else {
				cprintf(cid, "me: %s\n", line);
			}
		}
	}

	return;
}

struct game chat_game = {
	.name = "chat",
	.init = chat_init,
	.process = chat_process,
};
