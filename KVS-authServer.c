#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

typedef struct package {
	char key[1024];
	char value[1024];
} Package;

#define MAX_CONNECTIONS 10

int main() {
	pthread_t local_servers[MAX_CONNECTIONS];
	struct sockaddr_in server, client;
	int fd, client_address_size;
	char buf[32];

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd < 0) {
		printf("Error while creating socket\n");
		return -1;
	}
	printf("Socket created successfully\n");

	server.sin_family = AF_INET;
	server.sin_port = htons(8080);
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd, (const struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Couldn't bind to the port\n");
		return -1;
	}
	printf("Done with binding\n");

	client_address_size = sizeof(client);

	while (1) {
		if (recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &client_address_size) < 0) {
			perror("recvfrom()");
			exit(4);
		}

		printf("Received: %s\n", buf);
	}

	close(fd);

	return 0;
}