#ifndef CLIENT_H
#define CLIENT_H

enum state {
	FRESH = 0,
};

struct client_state {
	enum state state;
};

/*
 * client_init(): initialize the client state
 * @state: an allocated state
 *
 * May use cprintf() and cflush().
 */
void
client_init(struct client_state *state);

/*
 * client_process(): process a line from the client
 * @state: the client state
 * @line: its input line
 *
 * May use cprintf() and cflush().
 *
 * Return: 0 for success and -1 for failure
 */
int
client_process(struct client_state *state, char *line);

/*
 * client_clean(): cleans the client state
 * @state: the state to clean
 *
 * Does not free @state since client_init() did not allocate it.
 */
void
client_clean(struct client_state *state);

#endif /* client.h */
