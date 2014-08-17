#include <common.h>
#include <stdlib.h>
#include <string.h>
#include <game/chat.h>

#define ID_SLOTS 10
struct chat {
	int ids[ID_SLOTS];
};

static void *
chat_init(int id)
{
	struct chat *chat;
	int i;

	chat = malloc(sizeof(*chat));
	chat->ids[0] = id;
	for (i = 1; i < ID_SLOTS; i++)
		chat->ids[i] = -1;

	cprintf("Start chat.\n");

	return NULL;
}

static void
chat_process(void *data, char *line)
{
	struct chat *chat = data;
	(void)chat;

	cprintf("%s\n", line);

	return;
}

struct game chat_game = {
	.name = "chat",
	.init = chat_init,
	.process = chat_process,
};
