#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void __attribute__((noreturn))
usage(int status)
{
	fputs("\
usage: chess { server | client } <host> <port>\n\
", stdout);
	exit(status);
}

void __attribute__((noreturn))
server(char *host, char *port);

void __attribute__((noreturn))
client(char *host, char *port)
{
	printf("client %s %s\n", host, port);
	exit(EXIT_SUCCESS);
}

int
main(int argc, char *argv[])
{
	if (argc == 2 && !strcmp("--help", argv[1]))
		usage(EXIT_SUCCESS);
	if (argc != 4)
		usage(EXIT_FAILURE);
	if (!strcmp("server", argv[1]))
		server(argv[2], argv[3]);
	if (!strcmp("client", argv[1]))
		client(argv[2], argv[3]);
	usage(EXIT_FAILURE);
}
