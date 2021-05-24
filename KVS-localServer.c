#include "linkedList-lib.h"

#include "defs.h"

Node *head;

char *randomString(int size) {
	char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890\0";
	char *randString = calloc(size, sizeof(char));
	for (int i = 0; i < size; i++) {
		randString[i] = chars[rand() % strlen(chars)];
	}
	randString[size + 1] = '\0';
	return randString;
}

void *server_UI(void *arg) {
	Server_info_pack auth_server = *(Server_info_pack *)arg;

	int option = 0;
	Auth_Package pack;
	while (1) {
		printf("\n\tKVS Local Server\n");
		printf("1. Create Group\n2. Delete Group\n3. Show group info\n");
		printf("4. Show app status\n5. Exit\n");
		printf("\tChoose one option: ");
		scanf("%d", &option);
		if (option == 1) {
			printf("Group identifier: ");
			scanf("%s", pack.groupID);

			strcpy(pack.secret, randomString(5));
			printf("Secret: %s\n", pack.secret);

			pack.mode = 1;	// mode 1 ==> creating new group
			// Group created, send info to auth server
			sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

			recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
			printf("Received: %s\n", pack.groupID);
		} else if (option == 2) {  // Delete group
			printf("Group ID to delete: ");
			scanf("%s", pack.groupID);
			pack.mode = 2;
			sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

			recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
			printf("Received: %s\n", pack.groupID);
		} else if (option == 3) {  // Show group info
			printf("Group ID to view info: ");
			scanf("%s", pack.groupID);
			pack.mode = 3;
			sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

			recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
			printf("Received: \nGroupID: %s\nSecret: %s\n", pack.groupID, pack.secret);
		} else if (option == 4) {  // Show app status
								   /* code */
		} else if (option == 5) {
			break;
		}
	}
}

void *func(void *arg) {
	int client = *(int *)arg;
	Package client_package;
	while (1) {
		int n = recv(client, (void *)&client_package, sizeof(client_package), 0);
		printf("Received %d bytes, mode: %d key: %s value: %s\n", n, client_package.mode, client_package.key, client_package.value);
		if (client_package.mode == 1 && n > 0) {  // PUT VALUE
			if (searchNode(client_package.key, head) == NULL) {
				head = insertNode(client_package.key, client_package.value, head);
			} else {
				updateValue(searchNode(client_package.key, head), client_package.value);
			}
			strcpy(client_package.key, "accepted");
			send(client, (void *)&client_package, sizeof(client_package), 0);
		} else if (client_package.mode == 0 && n > 0) {	 // GET VALUE
			Node *aux = searchNode(client_package.key, head);
			if (aux == NULL) {
				strcpy(client_package.key, "declined");
			} else {
				strcpy(client_package.value, aux->value);
				strcpy(client_package.key, "accepted");
			}
			send(client, (void *)&client_package, sizeof(client_package), 0);
		} else if (client_package.mode == 2 && n > 0) {	 // DELETE VALUE
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
	srand(time(NULL));
	pthread_t client_threads[MAX_CONNECTIONS];
	int client_thread_status[MAX_CONNECTIONS];
	pthread_t auth_server_thread;

	head = create_LinkedList();
	Package client_package;
	Auth_Package pack;

	//////////client -- localserver socket///////////////////////
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
	//////////client -- localserver socket///////////////////////

	//////////localserver -- authserver socket///////////////////////
	int auth_socket = 0;
	if ((auth_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Auth socket creation");
		exit(1);
	}

	struct sockaddr_in endereco_auth;
	endereco_auth.sin_family = AF_INET;
	endereco_auth.sin_port = htons(8080);
	inet_aton("127.0.0.1", &endereco_auth.sin_addr);
	printf("Auth server socket created!! :)");

	Server_info_pack auth_server_info;
	auth_server_info.socket = auth_socket;
	auth_server_info.adress = endereco_auth;
	//////////localserver -- authserver socket///////////////////////

	pthread_t serverUI_thread;
	pthread_create(&serverUI_thread, NULL, (void *)server_UI, (void *)&auth_server_info);

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

			// authenticate with auth server
			strcpy(pack.groupID, client_package.value);
			strcpy(pack.secret, client_package.key);
			pack.mode = 10;
			sendto(auth_socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&endereco_auth, sizeof(endereco_auth));

			recv(auth_socket, (void *)&pack, sizeof(pack), 0);
			if (pack.mode == 1) {  // authentication was ok
				printf("Autenticated sucessfully\n");
				// comunicate to client
				strcpy(client_package.key, "accepted");
				send(client, (void *)&client_package, sizeof(client_package), 0);

				//the kvslib will send the client PID
				//recv(auth_socket, (void *)&pack, sizeof(pack), 0);
				break;
			} else {
				if (strcmp(pack.groupID, "accepted-group") == 0) {
					if (strcmp(pack.secret, "declined-key") == 0) {	 // correct group but wrong key
						strcpy(client_package.key, "accepted-group");
						strcpy(client_package.value, "declined-key");
					}
				} else if (strcmp(pack.groupID, "declined-group") == 0) {
					strcpy(client_package.key, "declined-group");
				}
			}

			send(client, (void *)&client_package, sizeof(client_package), 0);
		}

		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (client_thread_status[i] == 0) {
				client_thread_status[i] = 1;
				pthread_create(&client_threads[i], NULL, (void *)func, (void *)&client);
				break;
			}
		}
		// nao ha conexões suficientes
	}

	close(send_socket);
	unlink(SOCKNAME);
}