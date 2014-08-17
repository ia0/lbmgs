#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <poll.h>

#include <common.h>

struct client {
	FILE *stream;
	struct pollfd *poll;
};

#define SLOTS 20
static struct client clients[SLOTS];
static struct pollfd polls[SLOTS];

/* Arguments of getline(). */
static char *line;
static size_t linelen;

static void __attribute__((noreturn))
usage(int status)
{
	printf("usage: chess <host> <port>\n");
	exit(status);
}

/*
 * listen_on(): create, bind, and listen
 * @shost: hostname to listen to
 * @sport: port to listen to
 *
 * Return: the socket file descriptor
 */
static int
listen_on(char *shost, char *sport)
{
	int err;
	struct addrinfo hints = {0};
	struct addrinfo *res;
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];
	int sfd;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	err = getaddrinfo(shost, sport, &hints, &res);
	if (err)
		die_("getaddrinfo: %s\n", gai_strerror(err));

	if (!res)
		die("Incorrect host and port.\n");

	sfd = socket(res->ai_family, res->ai_socktype,
		    res->ai_protocol);
	if (sfd < 0)
		die_("socket: %s\n", strerror(errno));

	err = bind(sfd, res->ai_addr, res->ai_addrlen);
	if (err)
		die_("bind: %s\n", strerror(errno));

	err = getnameinfo(res->ai_addr, res->ai_addrlen,
			  host, sizeof(host), port, sizeof(port),
			  NI_NUMERICHOST | NI_NUMERICSERV);
	if (err)
		die_("getnameinfo: %s\n", gai_strerror(err));

	freeaddrinfo(res);

	err = listen(sfd, 0);
	if (err)
		die_("listen: %s\n", strerror(errno));

	printf("Listening on %s:%s.\n", host, port);

	return sfd;
}

/*
 * nonblock(): set the O_NONBLOCK status flag
 * @fd: the file descriptor
 *
 * Return: 0 for success and -1 for failure
 */
int
nonblock(int fd)
{
	int flags;
	flags = fcntl(fd, F_GETFL);
	if (flags < 0) {
		eprintf("%d: fcntl(F_GETFL): %s\n", fd, strerror(errno));
		return -1;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		eprintf("%d: fcntl(F_SETFL): %s\n", fd, strerror(errno));
		return -1;
	}
	return 0;
}

/* clean(): free allocated data */
static void __attribute__((noreturn))
clean(int status)
{
	int i;
	for (i = 0; i < 4; i++)
		close(i);
	for (i = 4; i < SLOTS; i++)
		if (clients[i].stream) {
			fprintf(clients[i].stream, "Shutting down.\n");
			fclose(clients[i].stream);
		}
	free(line);
	exit(status);
}

/* user(): handle commands from user */
static void
user(void)
{
	ssize_t read;

	while ((read = getline(&line, &linelen, stdin)) > 0) {
		if (line[read-1] == '\n')
			line[--read] = 0;

		if (!strcmp(line, "quit")) {
			clean(EXIT_SUCCESS);
		} else if (!strcmp(line, "help")) {
			printf("Available commands are:\n");
			printf("  help    print this help\n");
			printf("  quit    quit the server\n");
		} else {
			printf("Invalid command '%s' (try help).\n", line);
		}
	}
	if (feof(stdin))
		clean(EXIT_SUCCESS);
	assert(errno & (EAGAIN | EWOULDBLOCK));
	assert(ferror(stdin));
	clearerr(stdin);
}

/*
 * server(): accept a connection and setup the client
 * @sfd: the listening socket
 */
static void
server(int sfd)
{
	int err;
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];
	int fd;
	FILE *stream;

	fd = accept(sfd, (struct sockaddr *)&addr, &addrlen);
	if (fd < 0) {
		eprintf("accept: %s\n", strerror(errno));
		return;
	}

	err = getnameinfo((struct sockaddr *)&addr, addrlen,
			  host, sizeof(host), port, sizeof(port),
			  NI_NUMERICHOST | NI_NUMERICSERV);
	if (err) {
		eprintf("getnameinfo: %s\n", gai_strerror(err));
		goto close;
	}

	printf("%d: connected from %s:%s\n", fd, host, port);

	if (nonblock(fd))
		goto close;

	if (fd >= SLOTS) {
		const char msg[] = "Connection refused (no slots available).\n";
		assert(fd == SLOTS);
		if (write(fd, msg, sizeof(msg)) < 0)
			eprintf("%d: write: %s\n", fd, strerror(errno));
		printf("%d: rejected (no slots available)\n", fd);
		goto close;
	}

	assert(!clients[fd].stream);
	assert(polls[fd].fd == -1);

	/*
	 * From now on, only work with streams. The file descriptor is
	 * used for poll and logging.
	 */
	stream = fdopen(fd, "r+");
	if (!stream) {
		eprintf("%d: fdopen: %s\n", fd, strerror(errno));
		goto close;
	}
	fprintf(stream, "Hello %s:%s\n", host, port);
	fflush(stream);

	clients[fd].stream = stream;
	clients[fd].poll = &polls[fd];
	polls[fd].fd = fd;
	polls[fd].events = POLLIN;
	polls[fd].revents = 0;
	return;

close:
	if (close(fd))
		eprintf("%d: close: %s\n", fd, strerror(errno));
	return;
}

static void
close_client(struct client *c)
{
	printf("%d: closing\n", c->poll->fd);
	if (fclose(c->stream))
		eprintf("%d: fclose: %s\n", c->poll->fd, strerror(errno));
	c->stream = 0;
	c->poll->fd = -1;
	c->poll->events = 0;
	c->poll->revents = 0;
	c->poll = 0;
}

/* client(): handle commands from clients */
static void
client(struct client *c)
{
	ssize_t read;

	while ((read = getline(&line, &linelen, c->stream)) > 0) {
		if (line[read-1] != '\n')
			eprintf("%d: missing end of line\n", c->poll->fd);
		else
			line[--read] = 0;

		/* TODO: do the real stuff here */
		printf("%d: got '%s'\n", c->poll->fd, line);
	}
	if (feof(c->stream)) {
		printf("%d: eof\n", c->poll->fd);
		close_client(c);
		return;
	}
	assert(errno & (EAGAIN | EWOULDBLOCK));
	assert(ferror(c->stream));
	clearerr(c->stream);
}

int
main(int argc, char *argv[])
{
	int i;
	int sfd;

	/* Check command line arguments. */
	if (argc == 2 && !strcmp("--help", argv[1]))
		usage(EXIT_SUCCESS);
	if (argc != 3)
		usage(EXIT_FAILURE);

	/* Create listening file descriptor. */
	sfd = listen_on(argv[1], argv[2]);
	assert(sfd == 3);

	/* Initialize polls. */
	for (i = 0; i < 4; i++)
		polls[i].fd = i;
	polls[0].events = POLLIN;
	polls[3].events = POLLIN;
	nonblock(0);
	nonblock(3);
	for (i = 4; i < SLOTS; i++)
		polls[i].fd = -1;

#if DEBUG
	int count;
	for (count = 0; count < 12; count++) {
#else
	while (1) {
#endif
		int n;
		printf("> ");
		fflush(stdout);
		n = poll(polls, SLOTS, -1);
		if (n < 0)
			die_("poll: %s", strerror(errno));
#if DEBUG
		if (!n)
			eprintf("\rNo fds were polled.\n");
		else
#endif
			printf("\rPolled %d fd%s.\n", n, n > 1 ? "" : "s");

		/* Dispatch according to the file descriptor. */
		if (polls[0].revents)
			user();
		if (polls[3].revents)
			server(sfd);
		for (i = 4; i < SLOTS; i++)
			if (clients[i].stream && polls[i].revents)
				client(&clients[i]);
	}
#if DEBUG
	printf("End of loop.\n");
	clean(EXIT_SUCCESS);
#endif
}
