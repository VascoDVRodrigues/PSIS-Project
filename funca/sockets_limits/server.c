#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define SOCKNAME "/tmp/socket99"

int main() {
	struct sockaddr_un adress;
	int client_socket;

	adress.sun_family = AF_UNIX;
	strcpy(adress.sun_path, SOCKNAME);

	unlink(SOCKNAME);

	client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (client_socket < 0) {
		perror("Error creating stream socket");
		return -1;
	}

	if (bind(client_socket, (struct sockaddr *)&adress, sizeof(struct sockaddr_un))) {
		perror("Error binding stream socket");
		return -2;
	}

	printf("Binded!\n");

	listen(client_socket, 10);
	printf("Waiting for connections!!\n");
//---------------------------------------------------------------------------------------------------------------------------------------------------
	int newClient_socket = accept(client_socket, NULL, NULL);

	char *buff;

	size_t size;

	// primeiro receber o tamanho
	int a = recv(newClient_socket, (void *)&size, sizeof(size), 0);

	printf("Size received %ld\n", size);

	// buff = (char *)malloc(sizeof(char) * size);

	a = recv(newClient_socket, (void *)buff, size, 0);

	printf("Received: %s\n", buff);

	return 0;
}