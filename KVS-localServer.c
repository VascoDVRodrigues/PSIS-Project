#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "linkedList-lib.h"

#define SOCKNAME "/tmp/socket1"
#define MESSAGE_SIZE 100

typedef struct package {
	int mode;
	char key[MESSAGE_SIZE];
	char value[MESSAGE_SIZE];
} Package;

int main() {
	Node *head = create_LinkedList();

	struct sockaddr_un server;
	int send_socket;
	Package client_package;
	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, SOCKNAME);

	unlink(SOCKNAME);

	send_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (send_socket < 0) {
		perror("Error creating socket");
		exit(1);
	}

	if (bind(send_socket, (struct sockaddr *)&server, sizeof(struct sockaddr_un))) {
		perror("Error binding stream socket");
		exit(1);
	}

	printf("Socket created, has name %s\n", server.sun_path);

	listen(send_socket, 10);
	printf("Waiting for connections!!\n");
	int client = accept(send_socket, NULL, NULL);

	if (client != -1) {
		printf("Connected to %d\n", client);
	}
	while (1) {
		int n = recv(client, (void *)&client_package, sizeof(client_package), 0);
		printf("Received %d bytes, mode: %d key: %s value: %s\n", n, client_package.mode, client_package.key, client_package.value);
		if (client_package.mode == 1 && n > 0) {
			if (searchNode(client_package.key, head) == NULL) {
				head = insertNode(client_package.key, client_package.value, head);
			} else {
				updateValue(searchNode(client_package.key, head), client_package.value);
			}
			strcpy(client_package.key, "accepted");
		} else if (client_package.mode == 0 && n > 0) {
			Node *aux = searchNode(client_package.key, head);
			if (aux == NULL) {
				strcpy(client_package.key, "declined");
			} else {
				strcpy(client_package.value, aux->value);
				strcpy(client_package.key, "accepted");
			}
		} else if (client_package.mode == 2 && n > 0) {
			if (searchNode(client_package.key, head) == NULL) {
				strcpy(client_package.key, "declined");
			} else {
				deleteNode(client_package.key, head);
				strcpy(client_package.key, "accepted");
			}
		} else {
			strcpy(client_package.key, "declined");
		}

		send(client, (void *)&client_package, sizeof(client_package), 0);
		printList(head);
	}

	close(send_socket);
	unlink(SOCKNAME);
}