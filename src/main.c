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
#include <errno.h>
#include <stdarg.h>
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
#include <client.h>

char *program = "lbmgs";

FILE *streams[SLOTS];
struct client clients[SLOTS];
static struct pollfd polls[SLOTS];

/* Arguments of getline(). */
static char *line;
static size_t linelen;

static void __attribute__((noreturn))
usage(int status)
{
	printf("usage: %s <host> <port>\n", program);
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
	struct addrinfo hints;
	struct addrinfo *res;
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];
	int sfd;

	hints.ai_flags = 0;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;
	err = getaddrinfo(shost, sport, &hints, &res);
	if (err)
		die_("getaddrinfo: %s\n", gai_strerror(err));

	if (res == NULL)
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
static int
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

/* close_client(): free client allocated data */
static void
close_client(int cid)
{
	FILE *cstream;

	cstream = streams[cid];

	gprintf(cid, "closing\n");
	client_clean(cid);
	polls[cid].fd = -1;
	polls[cid].events = 0;
	polls[cid].revents = 0;
	if (fclose(cstream))
		geprintf(cid, "fclose: %s\n", strerror(errno));
	streams[cid] = NULL;

	return;
}

/* clean(): free all allocated data */
static void __attribute__((noreturn))
clean(int status)
{
	int i;

	for (i = 0; i < 4; i++)
		close(i);
	for (i = 4; i < SLOTS; i++)
		if (streams[i] != NULL) {
			fprintf(streams[i], "Server is shutting down.\n");
			fflush(streams[i]);
			close_client(i);
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
			line[--read] = '\0';

		if (!strcmp(line, "quit")) {
			clean(EXIT_SUCCESS);
		} else if (!strcmp(line, "help")) {
			printf("Available commands are:\n");
			printf("  quit    quit the server\n");
			printf("  help    print this help\n");
		} else {
			printf("Invalid command '%s' (try help).\n", line);
		}
	}
	if (feof(stdin))
		clean(EXIT_SUCCESS);

	assert(errno & (EAGAIN | EWOULDBLOCK));
	assert(ferror(stdin));
	clearerr(stdin);

	return;
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
	FILE *cstream;
	int cfd;

	cfd = accept(sfd, (struct sockaddr *)&addr, &addrlen);
	if (cfd < 0) {
		eprintf("accept: %s\n", strerror(errno));
		return;
	}

	err = getnameinfo((struct sockaddr *)&addr, addrlen,
			  host, sizeof(host), port, sizeof(port),
			  NI_NUMERICHOST | NI_NUMERICSERV);
	if (err) {
		geprintf(cfd, "getnameinfo: %s\n", gai_strerror(err));
		goto close;
	}

	gprintf(cfd, "connected from %s:%s\n", host, port);

	if (nonblock(cfd))
		goto close;

	if (cfd >= SLOTS) {
		const char msg[] = "Connection refused (no slots available).\n";
		assert(cfd == SLOTS);
		if (write(cfd, msg, sizeof(msg)) < 0)
			geprintf(cfd, "write: %s\n", strerror(errno));
		gprintf(cfd, "rejected (no slots available)\n");
		goto close;
	}

	assert(streams[cfd] == NULL);
	assert(polls[cfd].fd == -1);

	cstream = fdopen(cfd, "r+");
	if (cstream == NULL) {
		geprintf(cfd, "fdopen: %s\n", strerror(errno));
		goto close;
	}
	fprintf(cstream, "Hello %s:%s\n", host, port);
	fflush(cstream);

	streams[cfd] = cstream;
	polls[cfd].fd = cfd;
	polls[cfd].events = POLLIN;
	polls[cfd].revents = 0;
	client_init(cfd);

	return;

close:
	if (close(cfd))
		geprintf(cfd, "close: %s\n", strerror(errno));
	return;
}

/* client(): handle commands from clients */
static void
client(int cid)
{
	ssize_t read;
	FILE *cstream;

	cstream = streams[cid];

	while ((read = getline(&line, &linelen, cstream)) > 0) {
		if (line[read-1] != '\n')
			geprintf(cid, "missing end of line\n");
		else
			line[--read] = '\0';

		gprintf(cid, "got [4m%s[m\n", line);
		if (client_process(cid, line) < 0)
			goto close;
	}
	if (feof(cstream)) {
		gprintf(cid, "eof\n");
		goto close;
	}

	assert(errno & (EAGAIN | EWOULDBLOCK));
	assert(ferror(cstream));
	clearerr(cstream);

	return;

close:
	close_client(cid);
	return;
}

int
main(int argc, char *argv[])
{
	int i;
	int sfd;

	/* Check command line arguments. */
	program = argv[0];
	if (argc == 2 && !strcmp("--help", argv[1]))
		usage(EXIT_SUCCESS);
	if (argc != 3)
		usage(EXIT_FAILURE);

	/* Create listening file descriptor. */
	sfd = listen_on(argv[1], argv[2]);
	assert(sfd == 3);

	/* Initialize streams and clients. */
	for (i = 0; i < SLOTS; i++) {
		streams[i] = NULL;
		clients[i].party = -1;
	}

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
	for (count = 0; count < 20; count++) {
#else
	while (1) {
#endif
		int n;

		printf("> ");
		fflush(stdout);

		n = poll(polls, SLOTS, -1);
		if (n < 0)
			die_("poll: %s", strerror(errno));
		if (!n)
			eprintf("\rNo fds were polled.\n");
		else
			printf("\rPolled %d fd%s.\n", n, n > 1 ? "" : "s");

		/* Dispatch according to the file descriptor. */
		if (polls[0].revents)
			user();
		if (polls[3].revents)
			server(sfd);
		for (i = 4; i < SLOTS; i++)
			if (polls[i].events && polls[i].revents)
				client(i);
	}
#if DEBUG
	printf("End of loop.\n");
	clean(EXIT_SUCCESS);
#endif
}
