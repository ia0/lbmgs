#ifndef CLIENT_H
#define CLIENT_H

struct client {
	int party; /* -1 for lobby */
};

#define SLOTS 20
extern struct client clients[SLOTS];

#ifndef NDEBUG
static inline int
valid_cid(int cid)
{
	return 0 <= cid && cid < SLOTS;
}
#endif

/* cprintf(): print to a client */
int __attribute__((format(printf, 2, 3)))
cprintf(int cid, char *fmt, ...);

/* cflush(): flush the stream of a client */
void
cflush(int cid);

/* client_init(): initialize the client */
void
client_init(int cid);

/* client_process(): process a line from the client */
int /* success == 0, failure == -1 */
client_process(int cid, char *line);

/* client_clean(): cleans the client state */
void
client_clean(int cid);

#endif /* client.h */
