#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netdb.h>

#include <common.h>

void __attribute__((noreturn))
client(char *shost, char *sport)
{
	int err;
	struct addrinfo hints = {0};
	struct addrinfo *res;
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];
	int fd;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	err = getaddrinfo(shost, sport, &hints, &res);
	if (err)
		die_("getaddrinfo: %s\n", gai_strerror(err));

	if (!res)
		die("Incorrect host and port.\n");

	fd = socket(res->ai_family, res->ai_socktype,
		    res->ai_protocol);
	if (fd < 0)
		die_("socket: %s\n", strerror(errno));

	err = connect(fd, res->ai_addr, res->ai_addrlen);
	if (err)
		die_("connect: %s\n", strerror(errno));

	err = getnameinfo(res->ai_addr, res->ai_addrlen,
			  host, sizeof(host), port, sizeof(port),
			  NI_NUMERICHOST | NI_NUMERICSERV);
	if (err)
		die_("getnameinfo: %s", gai_strerror(err));

	printf("Connected to %s:%s.\n", host, port);

	freeaddrinfo(res);

	close(fd);
	exit(EXIT_SUCCESS);
}
