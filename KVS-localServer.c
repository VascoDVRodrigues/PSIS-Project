#include "defs.h"

LinkedList *groupsList;

void printKV(Item item) {
	KeyValue data = *(KeyValue *)item;
	printf("%s|%s\n", data.key, data.value);
	return;
}

void freeKeyValue(Item item) {
	KeyValue *KV = (KeyValue *)item;
	free(KV->value);
	free(KV);
	return;
}

int compareKeys(Item k1, Item k2) {
	KeyValue data1 = *(KeyValue *)k1;
	KeyValue data2 = *(KeyValue *)k2;

	if (strcmp(data1.key, data2.key) == 0) {
		return 1;
	}

	return 0;
}

// Cria e retorna uma random string com tamanho size
char *randomString(int size) {
	char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890\0";
	char *randString = calloc(size, sizeof(char));
	for (int i = 0; i < size; i++) {
		randString[i] = chars[rand() % strlen(chars)];
	}
	randString[size + 1] = '\0';
	return randString;
}

/*
Sets up local server
	creates and binds socket to comunicate with clients
	creates socket to comunicate with authServer
Returns:
0: no errors
-1: Error creating stream socket
-2: Error binding stream socket
-3: Error Auth socket creation
*/
int setup_LocalServer(Server_info_pack *clients, Server_info_pack_INET *auth_server) {
	groupsList = createList();

	/// client -- localserver socket///////////////////////
	clients->adress.sun_family = AF_UNIX;
	strcpy(clients->adress.sun_path, SOCKNAME);

	unlink(SOCKNAME);

	clients->socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (clients->socket < 0) {
		perror("Error creating stream socket");
		return -1;
	}

	if (bind(clients->socket, (struct sockaddr *)&clients->adress, sizeof(struct sockaddr_un))) {
		perror("Error binding stream socket");
		return -2;
	}

	printf("Socket created, has name %s\n", clients->adress.sun_path);
	//////////client -- localserver socket///////////////////////

	//////////localserver -- authserver socket///////////////////////
	if ((auth_server->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Auth socket creation");
		return -3;
	}

	auth_server->adress.sin_family = AF_INET;
	auth_server->adress.sin_port = htons(8080);
	inet_aton("127.0.0.1", &auth_server->adress.sin_addr);
	printf("Auth server socket created!! :)");
	//////////localserver -- authserver socket///////////////////////
	return 0;
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

void *client_handler(void *arg) {
	Client_info client = *(Client_info *)arg;

	App_Package client_package;
	while (1) {
		int n = recv(client.socket, (void *)&client_package, sizeof(client_package), 0);
		printf("Received %d bytes, mode: %d key: %s value: %s\n", n, client_package.mode, client_package.key, client_package.value);
		if (client_package.mode == 1 && n > 0) {  // PUT VALUE
			KeyValue *newData = calloc(1, sizeof(KeyValue));
			strcpy(client_package.key, newData->key);
			newData->value = calloc(strlen(client_package.key), sizeof(char));
			strcpy(newData->value, client_package.value);

			if (searchNode(client.connected_group->keyValue_List, newData, compareKeys) == NULL) {
				// a key ainda n existe, guardar na lista de key|value
				client.connected_group->keyValue_List = insertNode(client.connected_group->keyValue_List, (Item)&newData);
			} else {
				// Neste caso data to update e o proprio update sao iguais, pois a comparacão e feita tendo por base apenas a key
				client.connected_group->keyValue_List =
					updateNode(client.connected_group->keyValue_List, (Item)newData, (Item)newData, compareKeys, freeKeyValue);
			}

			strcpy(client_package.key, "accepted");
			send(client.socket, (void *)&client_package, sizeof(client_package), 0);
		} else if (client_package.mode == 0 && n > 0) {	 // GET VALUE
			KeyValue data_to_find;
			strcpy(data_to_find.key, client_package.key);

			KeyValue *data = (KeyValue*)searchNode(client.connected_group->keyValue_List, (Item)&data_to_find, compareKeys);

			// SubNode *aux = searchNode(client_package.key, client.connected_group->GroupHead);
			if (data == NULL) {
				strcpy(client_package.key, "declined");
			} else {
				strcpy(client_package.value, data->value);
				strcpy(client_package.key, "accepted");
			}
			send(client.socket, (void *)&client_package, sizeof(client_package), 0);
		} else if (client_package.mode == 2 && n > 0) {	 // DELETE VALUE
			KeyValue data_to_delete;
			strcpy(data_to_delete.key, client_package.key);

			if (searchNode(client.connected_group->keyValue_List, (Item)&data_to_delete, compareKeys) == NULL) {
				strcpy(client_package.key, "declined");
			} else {
				client.connected_group->keyValue_List =
					deleteNode(client.connected_group->keyValue_List, (Item)&data_to_delete, compareKeys, freeKeyValue);
				strcpy(client_package.key, "accepted");
			}
			send(client.socket, (void *)&client_package, sizeof(client_package), 0);
		} else if (n == 0) {
			break;
		} else {
			printf("else\n");
			strcpy(client_package.key, "declined");
			send(client.socket, (void *)&client_package, sizeof(client_package), 0);
		}
		printList(client.connected_group->keyValue_List, printKV, 0);
	}

	return NULL;
}
// pthread_equal(pthread_t t1, pthread_t t2);
int main() {
	srand(time(NULL));
	pthread_t client_threads[MAX_CONNECTIONS];
	int client_thread_status[MAX_CONNECTIONS];
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		client_thread_status[i] = 0;
	}

	Server_info_pack clients;
	Server_info_pack_INET auth_server;
	setup_LocalServer(&clients, &auth_server);

	pthread_t serverUI_thread;
	pthread_create(&serverUI_thread, NULL, (void *)server_UI, (void *)&auth_server);

	listen(clients.socket, 10);
	printf("Waiting for connections!!\n");

	int newClient_socket = 0, n = 0;
	Auth_Package pack;
	App_Package client_package;
	while (1) {
		// while (1) {
		newClient_socket = accept(clients.socket, NULL, NULL);
		if (newClient_socket != -1) {
			printf("Connected to %d\n", newClient_socket);
		}

		n = recv(newClient_socket, (void *)&client_package, sizeof(client_package), 0);
		printf("Client %d wants to connect to group %s, with secret %s\n", newClient_socket, client_package.value, client_package.key);

		// authenticate with auth server
		strcpy(pack.groupID, client_package.value);
		strcpy(pack.secret, client_package.key);
		pack.mode = 10;
		sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

		// process response from auth server
		recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
		if (pack.mode == 1) {  // authentication was ok
			printf("Autenticated sucessfully\n");

			// Since the group exists we should add it to the groups list
			Grupo *newGroup = calloc(1, sizeof(Grupo));
			newGroup->keyValue_List = createList();	 // criar uma lista para guardar as key|value
			newGroup->n_keyValues = 0;
			strcpy(newGroup->groupID, client_package.value);
			groupsList = insertNode(groupsList, (Item)newGroup);

			// comunicate to client
			strcpy(client_package.key, "accepted");
			send(newClient_socket, (void *)&client_package, sizeof(client_package), 0);

			// the kvslib will send the client PID, in client_package.mode
			recv(newClient_socket, (void *)&client_package, sizeof(client_package), 0);

			// Contruir o package que vai ser enviado para a thread que trata deste cliente
			Client_info newClient;
			newClient.socket = newClient_socket;
			newClient.connected_group = newGroup;

			// Finally start the client handler thread
			for (int i = 0; i < MAX_CONNECTIONS; i++) {
				if (client_thread_status[i] == 0) {
					client_thread_status[i] = 1;
					pthread_create(&client_threads[i], NULL, (void *)client_handler, (void *)&newClient);
					break;
				}
			}
			// break;
		} else {  // authentication was not ok
			if (strcmp(pack.groupID, "accepted-group") == 0) {
				if (strcmp(pack.secret, "declined-key") == 0) {	 // correct group but wrong key
					strcpy(client_package.key, "accepted-group");
					strcpy(client_package.value, "declined-key");
				}
			} else if (strcmp(pack.groupID, "declined-group") == 0) {
				strcpy(client_package.key, "declined-group");
			}
			// comunicate to client
			send(newClient_socket, (void *)&client_package, sizeof(client_package), 0);
		}
		// }
	}

	close(clients.socket);
	unlink(SOCKNAME);
}