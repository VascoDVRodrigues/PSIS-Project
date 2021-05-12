#include <pthread.h>
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
#define MAX_CONNECTIONS 5

typedef struct package {
	int mode;
	char key[MESSAGE_SIZE];
	char value[MESSAGE_SIZE];
} Package;

Node *head;

void *func(void *arg) {
	int client = *(int *)arg;
	Package client_package;
	while (1) {
		printf("boas");
		int n = recv(client, (void *)&client_package, sizeof(client_package), 0);
		printf("Received %d bytes, mode: %d key: %s value: %s\n", n, client_package.mode, client_package.key, client_package.value);
		if (client_package.mode == 1 && n > 0) {
			if (searchNode(client_package.key, head) == NULL) {
				head = insertNode(client_package.key, client_package.value, head);
			} else {
				updateValue(searchNode(client_package.key, head), client_package.value);
			}
			strcpy(client_package.key, "accepted");
			send(client, (void *)&client_package, sizeof(client_package), 0);
		} else if (client_package.mode == 0 && n > 0) {
			Node *aux = searchNode(client_package.key, head);
			if (aux == NULL) {
				strcpy(client_package.key, "declined");
			} else {
				strcpy(client_package.value, aux->value);
				strcpy(client_package.key, "accepted");
			}
			send(client, (void *)&client_package, sizeof(client_package), 0);
		} else if (client_package.mode == 2 && n > 0) {
			if (searchNode(client_package.key, head) == NULL) {
				strcpy(client_package.key, "declined");
			} else {
				head = deleteNode(client_package.key, head);
				strcpy(client_package.key, "accepted");
			}
			send(client, (void *)&client_package, sizeof(client_package), 0);
		} else if (n == 0) {
			break;
		} else {
			printf("else\n");
			strcpy(client_package.key, "declined");
			send(client, (void *)&client_package, sizeof(client_package), 0);
		}
		printList(head);
	}
	return NULL;
}
// pthread_equal(pthread_t t1, pthread_t t2);
int main() {
	pthread_t client_threads[MAX_CONNECTIONS];
	int client_thread_status[MAX_CONNECTIONS];
	pthread_t auth_server_thread;
	char mock_secret[30];
	char mock_groupid[30];
	strcpy(mock_secret, "12345678");
	strcpy(mock_groupid, "admin");

	head = create_LinkedList();
	Package client_package;

	int send_socket;
	struct sockaddr_un server;
	int client, n;
	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, SOCKNAME);

	unlink(SOCKNAME);
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		client_thread_status[i] = 0;
	}

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

	while (1) {
		while (1) {
			client = accept(send_socket, NULL, NULL);
			if (client != -1) {
				printf("Connected to %d\n", client);
			}

			n = recv(client, (void *)&client_package, sizeof(client_package), 0);
			printf("Client %d wants to connect to group %s, with secret %s\n", client, client_package.value, client_package.key);

			if (strcmp(mock_groupid, client_package.value) != 0) {	// ver se o grupo existe
				strcpy(client_package.key, "declined-group");

			} else {
				if (strcmp(mock_secret, client_package.key) != 0) {	 // password errada
					strcpy(client_package.key, "declined-key");
				} else {
					strcpy(client_package.key, "accepted");
					send(client, (void *)&client_package, sizeof(client_package), 0);
					break;
				}
			}
			send(client, (void *)&client_package, sizeof(client_package), 0);
		}

		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (client_thread_status[i] == 0) {
				client_thread_status[i] = 1;
				pthread_create(&client_threads[i], NULL, (void *)func, (void *)&client);
			}
		}
		// nao ha conexões suficientes
	}

	close(send_socket);
	unlink(SOCKNAME);
}