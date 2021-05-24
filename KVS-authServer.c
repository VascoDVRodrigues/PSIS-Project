#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "linkedList-lib.h"

typedef struct package {
	char key[1024];
	char value[1024];
} Package;

typedef struct auth_package {
	char groupID[1024];
	char secret[1024];
	int mode;
} Auth_Package;

#define MAX_CONNECTIONS 10

Node *head;

int main() {
	pthread_t local_servers[MAX_CONNECTIONS];
	struct sockaddr_in server, client;
	int fd, client_address_size;
	char buf[32];
	Auth_Package pack;

	head = create_LinkedList();

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
		if (recvfrom(fd, &pack, sizeof(pack), 0, (struct sockaddr *)&client, &client_address_size) < 0) {
			perror("recvfrom()");
			exit(4);
		}

		printf("Received: \n\tgroupID:%s\n\tsecret:%s\n\tmode:%d\n", pack.groupID, pack.secret, pack.mode);

		if (pack.mode==1) { //a new group has been created, save the info
			/* code */
		} else if (pack.mode==2) {
			/* code */
		} else {
			/* code */
		}
	}

	close(fd);

	return 0;
}