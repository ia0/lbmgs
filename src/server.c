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

struct client {
	int fd;
};

struct client clients[5];

void __attribute__((noreturn))
server(char *shost, char *sport)
{
	int err;
	struct addrinfo hints = {0};
	struct addrinfo *res;
	struct sockaddr_storage addr;
	socklen_t addrlen = sizeof(addr);
	char host[NI_MAXHOST];
	char port[NI_MAXSERV];
	int sfd, cfd;

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
		die_("getnameinfo: %s", gai_strerror(err));

	freeaddrinfo(res);

	err = listen(sfd, 0);
	if (err)
		die_("listen: %s\n", strerror(errno));

	printf("Listening on %s:%s.\n", host, port);

	while (1) {
		cfd = accept(sfd, (struct sockaddr *)&addr, &addrlen);
		if (cfd < 0)
			die_("accept: %s", strerror(errno));

		err = getnameinfo((struct sockaddr *)&addr, addrlen,
				  host, sizeof(host), port, sizeof(port),
				  NI_NUMERICHOST | NI_NUMERICSERV);
		if (err)
			die_("getnameinfo: %s", gai_strerror(err));

		if (cfd < (int)(sizeof(clients)/sizeof(clients[0]))) {
			clients[cfd].fd = cfd;
			printf("Got a connection (%d) from %s:%s.\n", cfd, host, port);
		} else {
			char msg[] = "Connection refused (no slots available).\n";
			write(cfd, msg, sizeof(msg));
			close(cfd);
			printf("Rejected a connection (%d) from %s:%s.\n", cfd, host, port);
		}
	}
}
