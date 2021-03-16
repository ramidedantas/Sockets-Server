/*
 * Servidor simples usando sockets
 * Adaptado por Ramide Dantas (IFPE) do livro de Andrew S. Tanenbaum
 * 2019-11-11
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 12345 /* arbitrary, but client & server must agree */
#define BUF_SIZE 32 /* block transfer size */
#define QUEUE_SIZE 10

void fatal(char *string) {
	printf("%s\n", string);
	exit(1);
}

int main(int argc, char *argv[]) {
	int s, b, l, fd, sa, bytes, on = 1;

	char buf[BUF_SIZE]; /* buffer for outgoing file */
	struct sockaddr_in channel; /* holds IP address */
	/* Build address structure to bind to socket. */

	memset(&channel, 0, sizeof(channel)); /* zero channel */
	channel.sin_family = AF_INET;
	channel.sin_addr.s_addr = htonl(INADDR_ANY);
	channel.sin_port = htons(SERVER_PORT);

	setvbuf(stdout, NULL, _IONBF, 0);

	printf("Servidor: criando e configurando socket ...\n");

	/* Passive open. Wait for connection. */
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* create socket */
	if (s < 0)
		fatal("Servidor: ERRO: \"socket\" falhou!");

	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));

	printf("Servidor: ouvindo na porta %d\n", SERVER_PORT);

	b = bind(s, (struct sockaddr *) &channel, sizeof(channel));
	if (b < 0)
		fatal("Servidor: ERRO: \"bind\" falhou!");


	l = listen(s, QUEUE_SIZE); /* specify queue size */
	if (l < 0)
		fatal("Servidor: ERRO: \"listen\" falhou!");

	while (1) {
		printf("Servidor: esperando conexão ...\n");
		sa = accept(s, 0, 0); /* block for connection request */
		printf("Servidor: conexão realizada ...\n");

		if (sa < 0)
			fatal("Servidor: ERRO: \"accept\" falhou!");

		printf("Servidor: recebendo nome do arquivo ...\n");

		read(sa, buf, BUF_SIZE);

		printf("Servidor: abrindo arquivo \"%s\"\n", buf);
		fd = open(buf, O_RDONLY); /* open the file to be sent back */

		if (fd < 0)
			fatal("open failed");

		printf("Servidor: lendo e enviando dados ...\n");
		while (1) {
			bytes = read(fd, buf, BUF_SIZE); /* read from file */
			if (bytes <= 0)
				break; /* check for end of file */
			write(sa, buf, bytes); /* write bytes to socket */


			sleep(5); /* pause to check connection */
		}

		printf("Servidor: dados transmitidos, fechando conexão e arquivo ... ");

		close(fd); /* close file */
		close(sa); /* close connection */

		printf("OK\n");
	}
}
