#ifndef CLIENT_H
#define CLIENT_H

struct client_state {
	int id;
	int party;
};

/*
 * client_init(): initialize the client state
 * @state: an allocated state
 * @id:    client id
 */
void
client_init(struct client_state *state, int id);

/*
 * client_process(): process a line from the client
 * @state: the client state
 * @line: its input line
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
