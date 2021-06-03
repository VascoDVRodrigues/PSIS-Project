#define _GNU_SOURCE
// por alguma razao extremamente desconhecida por isto aqui retira o warning do vscode a dizer q o rw_lock nao existe....

#include "defs.h"

Server_info_pack_INET auth_server;

Server_info_pack callbacks_server;
int callbackSockets[MAX_CONNECTIONS];

LinkedList *groupsList;

// List with all apps, previously connected and currently connected
LinkedList *apps;

int client_thread_status[MAX_CONNECTIONS];

pthread_rwlock_t Group_acess;  // serve para bloquear o acesso a todos os grupos

pthread_rwlock_t Apps_acess;

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

void freeGroup(Item item) {
	Grupo *g = (Grupo *)item;

	g->keyValue_List = clearList(g->keyValue_List, freeKeyValue);

	free(g);
}

int compareGroups(Item g1, Item g2) {
	Grupo data1 = *(Grupo *)g1;
	Grupo data2 = *(Grupo *)g2;

	if (strcmp(data1.groupID, data2.groupID) == 0) {
		return 1;
	}
}

int compareKeys(Item k1, Item k2) {
	KeyValue data1 = *(KeyValue *)k1;
	KeyValue data2 = *(KeyValue *)k2;

	if (strcmp(data1.key, data2.key) == 0) {
		return 1;
	}

	return 0;
}

int compareApps(Item a1, Item a2) {
	AppInfo A1 = *(AppInfo *)a1;
	AppInfo A2 = *(AppInfo *)a2;

	if (A1.PID == A2.PID) {
		return 1;
	}
	return 0;
}

void printApp(Item a) {
	AppInfo A = *(AppInfo *)a;

	// nao colocar a func ctime_r() dentro do printf, dá um output errado
	// nao alterar o tamanho das strings senao da core dumped
	// as strings teem 26 pq segundo a documentacao é o tamanho da string retornada pelo ctime_r
	// NÃO MUDAR PARA A FUNC ctime() PORQUE NAO É THREAD SAFE
	char start[26], end[26];

	if (ctime_r(&A.start, start) == NULL) {
		perror("ctime_r func");
	}

	if (ctime_r(&A.end, end) == NULL) {
		perror("ctime_r func");
	}

	printf("App %d\n is ", A.PID);
	if (A.connected) {
		printf("currently connected\n Started connection at %s\n", start);
	} else {
		printf("disconnected.\n Started connection at %s\nEnded connection at %s\n", start, end);
	}
	return;
}

void freeApps(Item a1) {
	// free(a1);
	return;
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
int setup_LocalServer(Server_info_pack *clients) {
	groupsList = createList();
	apps = createList();

	pthread_rwlock_init(&Group_acess, NULL);
	pthread_rwlock_init(&Apps_acess, NULL);

	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		client_thread_status[i] = 0;
		callbackSockets[i] = 0;
	}

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
	if ((auth_server.socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Auth socket creation");
		return -3;
	}

	auth_server.adress.sin_family = AF_INET;
	auth_server.adress.sin_port = htons(8080);
	inet_aton(AUTHSERVER_IP, &auth_server.adress.sin_addr);

	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	// man setsockopt e man 7 socket para perceber as flags
	setsockopt(auth_server.socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);
	printf("Auth server socket created!! :)");
	//////////localserver -- authserver socket///////////////////////

	// local-server --> kvslib CALBACK SOCKET/////////////////
	callbacks_server.adress.sun_family = AF_UNIX;
	strcpy(callbacks_server.adress.sun_path, CALLBACKS_SOCK);

	unlink(CALLBACKS_SOCK);

	callbacks_server.socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (callbacks_server.socket < 0) {
		perror("Error creating stream socket");
		return -1;
	}

	if (bind(callbacks_server.socket, (struct sockaddr *)&callbacks_server.adress, sizeof(struct sockaddr_un))) {
		perror("Error binding stream socket");
		return -2;
	}

	printf("Socket created, has name %s\n", callbacks_server.adress.sun_path);
	////////////////////////////////////////////////////////////////////////////////

	return 0;
}

void *server_UI(void *arg) {
	Server_info_pack auth_server = *(Server_info_pack *)arg;

	int option = 0;
	int err = 0;
	Package pack;
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

			// Para simular o caso improvavel de alguem tentar conectar-se com a password
			// certa enquanto o grupo esta a ser criado
			// strcpy(pack.secret, "boi");

			printf("Secret: %s\n", pack.secret);

			pack.mode = 1;	// mode 1 ==> creating new group
			// Group created, send info to auth server
			pthread_rwlock_wrlock(&Group_acess);

			sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

			err = recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
			if (err < 0) {
				printf("Timed out, try again later please\n");
			} else {
				printf("Received: %s\n", pack.groupID);
			}
			pthread_rwlock_unlock(&Group_acess);
		} else if (option == 2) {  // Delete group
			printf("Group ID to delete: ");
			scanf("%s", pack.groupID);
			pack.mode = 2;
			Grupo G_to_delete;
			strcpy(G_to_delete.groupID, pack.groupID);

			pthread_rwlock_wrlock(&Group_acess);
			printf("REgiao critica do delete group\n");
			sleep(10);
			sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

			err = recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
			if (err < 0) {
				printf("Timed out, try again later please\n");
			} else {
				printf("Received: %s\n", pack.groupID);
				// pesquisar na lista de grupos e colocar como desconectado
				Grupo *g_delete;
				g_delete = (Grupo *)searchNode(groupsList, (Item)&G_to_delete, compareGroups);
				if (g_delete != NULL) {
					printf("g_delete: %p\n", g_delete);
					g_delete->deleted = 1;
				}
			}
			printf("\n\tleaving crit reagion\n");
			pthread_rwlock_unlock(&Group_acess);
		} else if (option == 3) {  // Show group info
			printf("Group ID to view info: ");
			scanf("%s", pack.groupID);
			pack.mode = 3;

			// search this group in the groups list to display #keys|values
			Grupo grupo_to_find;
			strcpy(grupo_to_find.groupID, pack.groupID);

			sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

			err = recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
			if (err < 0) {
				printf("Timed out, try again later please\n");
			} else {
				// printf("Received: %s\n", pack.groupID);

				printf("Received: \n\tGroupID: %s\n\tSecret: %s\n", pack.groupID, pack.secret);
				pthread_rwlock_rdlock(&Group_acess);
				Grupo *grupo_found = (Grupo *)searchNode(groupsList, (Item)&grupo_to_find, compareGroups);

				if (grupo_found == NULL) {	// se o grupo ainda n esta na lista de grupos neste server é pq ainda n esta a guardar nada
					printf("\tThis group doesn't exist here :(\n");
				} else {
					printf("\tHas %d keys|values\n", grupo_found->n_keyValues);
				}
				pthread_rwlock_unlock(&Group_acess);
			}

		} else if (option == 4) {  // Show app status
			pthread_rwlock_rdlock(&Apps_acess);
			printList(apps, printApp, 0);
			pthread_rwlock_unlock(&Apps_acess);
		} else if (option == 5) {
			clearList(groupsList, freeGroup);
			clearList(apps, freeApps);
			for (int i = 0; i < MAX_CONNECTIONS; i++) {
				close(callbackSockets[i]);
			}
			close(auth_server.socket);
			exit(1);
			break;
		}
	}
}

void *callback_handler(void *arg) {
	char *alteredKey = (char *)arg;

	printf("\t\tI will send a message to all clients callback socks\n");
	printf("Because key %s was altered\n", alteredKey);
	// sleep(10);

	Package pack;
	strcpy(pack.secret, alteredKey);
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		if (callbackSockets[i] != 0) {
			send(callbackSockets[i], (void *)&pack, sizeof(pack), 0);
			printf("\t\tdone\n");
		}
	}
}

void *client_handler(void *arg) {
	Client_info client = *(Client_info *)arg;
	int n = 0;
	Package pack;
	AppInfo *new_app;
	pthread_t callback_th;
	int new_callback_sock;

	n = recv(client.socket, (void *)&pack, sizeof(pack), 0);
	printf("Client %d wants to connect to group %s, with secret %s\n", client.socket, pack.groupID, pack.secret);

	pthread_rwlock_wrlock(&Group_acess);  // a autenticacao comeca aqui
	// printf("\n\tdentro da regiao critica da autenticacao\n");

	// sleep(10);
	// como o package e o mesmo e so mudar o modo
	pack.mode = 10;
	sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

	// process response from auth server
	int x = recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
	if (x < 0) {
		printf("TIMED OUT\n");
		pack.mode = 0;
		strcpy(pack.groupID, "timed-out");
	}

	if (pack.mode == 1) {  // authentication was ok
		printf("Autenticated sucessfully\n");

		// Since the group exists we should add it to the groups list
		Grupo *newGroup = calloc(1, sizeof(Grupo));
		newGroup->keyValue_List = createList();	 // criar uma lista para guardar as key|value
		newGroup->n_keyValues = 0;
		newGroup->connected = 0;
		newGroup->deleted = 0;
		strcpy(newGroup->groupID, pack.groupID);

		// ver se o grupo ja foi adicionado por outra app

		if (searchNode(groupsList, (Item)newGroup, compareGroups) == NULL) {  // aka ainda n existe
			groupsList = insertNode(groupsList, (Item)newGroup);
		}

		// comunicate to client, inside secret
		strcpy(pack.secret, "accepted");
		send(client.socket, (void *)&pack, sizeof(pack), 0);

		// the kvslib will send the client PID, in pack.mode
		recv(client.socket, (void *)&pack, sizeof(pack), 0);
		new_app = (AppInfo *)calloc(1, sizeof(AppInfo));
		new_app->connected = 1;
		new_app->PID = pack.mode;
		new_app->start = time(NULL);
		apps = insertNode(apps, (Item)new_app);

		client.connected_group = (Grupo *)searchNode(groupsList, (Item)newGroup, compareGroups);
		client.connected_group->connected += 1;

		// wait for a new connect, but on the socket created for the callbacks
		new_callback_sock = accept(callbacks_server.socket, NULL, NULL);
		if (new_callback_sock != -1) {
			printf("Connected to %d\n", new_callback_sock);
		}
		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (callbackSockets[i] == 0) {
				callbackSockets[i] = new_callback_sock;
				client.callback_sock_index = i;
				break;
			}
		}
		pthread_rwlock_unlock(&Group_acess);
	} else {  // authentication was not ok
		pthread_rwlock_unlock(&Group_acess);
		printf("Authentication NOT sucessfull :(\n");
		if (strcmp(pack.groupID, "accepted-group") == 0) {
			if (strcmp(pack.secret, "declined-secret") == 0) {	// correct group but wrong key
				strcpy(pack.groupID, "accepted-group");
				strcpy(pack.secret, "declined-secret");
			}
		} else if (strcmp(pack.groupID, "declined-group") == 0) {
			strcpy(pack.secret, "declined-group");
		} else if (strcmp(pack.groupID, "timed-out") == 0) {
			strcpy(pack.secret, "timed-out");
		}

		// comunicate to client
		send(client.socket, (void *)&pack, sizeof(pack), 0);
		// Kill this thread
		client_thread_status[client.index_client_thread_status] = 0;
		pthread_exit((void *)1);
	}

	while (1) {
		n = recv(client.socket, (void *)&pack, sizeof(pack), 0);

		// printf("Received %d bytes, mode: %d groupID: %s secret: %s\n", n, pack.mode, pack.groupID, pack.secret);
		if (pack.mode == 1 && n > 0) {	// PUT VALUE
			KeyValue *newData = calloc(1, sizeof(KeyValue));

			// Cliente mandou a key no secret
			strcpy(newData->key, pack.secret);

			// Cliente vai mandar o tamanho do value e depois o value
			size_t size = 0;
			recv(client.socket, (void *)&size, sizeof(size_t), 0);

			newData->value = (char *)malloc(sizeof(char) * size);

			recv(client.socket, (void *)newData->value, size, 0);

			// sleep(5);
			pthread_rwlock_wrlock(&Group_acess);
			if ((client.connected_group->deleted == 1) && (client.connected_group->connected != 0)) {
				// apagar as variavais que nao vao ser usadas
				free(newData->value);
				free(newData);
			} else {
				KeyValue *found = (KeyValue *)searchNode(client.connected_group->keyValue_List, newData, compareKeys);
				if (found == NULL) {
					// a key ainda n existe, guardar na lista de key|value
					client.connected_group->keyValue_List = insertNode(client.connected_group->keyValue_List, (Item)newData);
					// incrementar o numero de keys|values que o grupo já tem
					client.connected_group->n_keyValues += 1;
				} else {
					// ver se o update é a mesma coisa do que ja la esta
					if (strcmp(found->value, newData->value) != 0) {
						// Neste caso data to update e o proprio update sao iguais, pois a comparacão e feita tendo por base apenas a key
						client.connected_group->keyValue_List =
							updateNode(client.connected_group->keyValue_List, (Item)newData, (Item)newData, compareKeys, freeKeyValue);

						// A KEY FOI UPDATED AQUI
						// SINALIZAR TODAS AS APPS LIGADAS
						pthread_create(&callback_th, NULL, (void *)callback_handler, (void *)&newData->key);
					}
				}

				strcpy(pack.secret, "accepted");
				send(client.socket, (void *)&pack, sizeof(pack), 0);
			}
			pthread_rwlock_unlock(&Group_acess);
		} else if (pack.mode == 0 && n > 0) {  // GET VALUE
			KeyValue data_to_find;

			// cliente mandou a key no pack
			strcpy(data_to_find.key, pack.secret);

			pthread_rwlock_rdlock(&Group_acess);
			if (!((client.connected_group->deleted == 1) && (client.connected_group->connected != 0))) {
				KeyValue *data = (KeyValue *)searchNode(client.connected_group->keyValue_List, (Item)&data_to_find, compareKeys);

				pthread_rwlock_unlock(&Group_acess);
				// SubNode *aux = searchNode(pack.key, client.connected_group->GroupHead);
				if (data == NULL) {
					strcpy(pack.secret, "declined");
					send(client.socket, (void *)&pack, sizeof(pack), 0);
				} else {
					// Como encontramos vamos enviar esta informacao
					strcpy(pack.secret, "accepted");
					send(client.socket, (void *)&pack, sizeof(pack), 0);

					// agora vamos enviar o tamanho do value e depois o value
					size_t size = strlen(data->value) + 1;
					send(client.socket, (void *)&size, sizeof(size), 0);

					send(client.socket, (void *)data->value, size, 0);
				}
				// else do nothing
			} else {
				pthread_rwlock_unlock(&Group_acess);
			}

		} else if (pack.mode == 2 && n > 0) {  // DELETE VALUE
			KeyValue data_to_delete;
			strcpy(data_to_delete.key, pack.secret);
			pthread_rwlock_wrlock(&Group_acess);
			if (!((client.connected_group->deleted == 1) && (client.connected_group->connected != 0))) {
				if (searchNode(client.connected_group->keyValue_List, (Item)&data_to_delete, compareKeys) == NULL) {
					strcpy(pack.secret, "declined");
				} else {
					client.connected_group->keyValue_List =
						deleteNode(client.connected_group->keyValue_List, (Item)&data_to_delete, compareKeys, freeKeyValue);
					strcpy(pack.secret, "accepted");
				}
				send(client.socket, (void *)&pack, sizeof(pack), 0);
			}
			pthread_rwlock_unlock(&Group_acess);  // else do nothing
		} else if (pack.mode == 3 || n >= 0) {	  // close connection
			client.connected_group->connected -= 1;
			/*
			// kvslib will send the app's PID, inside variable mode
			recv(client.socket, (void *)&pack, sizeof(pack), 0);*/
			// this app is closing so find the app in the apps list and set that to disconnected
			AppInfo *app_closing = (AppInfo *)calloc(1, sizeof(AppInfo));

			// app_closing->PID = pack.mode;
			app_closing->PID = new_app->PID;
			// basta enviar a struct com o PID preenchido pois a comparacao e feita tendo por base o PID apenas
			pthread_rwlock_wrlock(&Apps_acess);
			app_closing = (AppInfo *)searchNode(apps, (Item)app_closing, compareApps);

			if (app_closing == NULL) {
				printf("Algo de muito errado nao esta certo...\n");
			}

			// dar update da flag e colocar data em q desligou
			app_closing->end = time(NULL);
			app_closing->connected = 0;

			// atualizar na lista
			// envia-se no data to update e no proprio update a mm struct pois a comparacao e feita so com o PID
			apps = updateNode(apps, (Item)app_closing, (Item)app_closing, compareApps, freeApps);
			pthread_rwlock_unlock(&Apps_acess);
			// finally set this tread as free
			client_thread_status[client.index_client_thread_status] = 0;  // POSSIVEL MUTEX???
			callbackSockets[client.callback_sock_index] = 0;
			break;
		} else if (pack.mode == 4 && n > 0) {  // register callback
			KeyValue data_to_find;

			// cliente mandou a key no pack
			strcpy(data_to_find.key, pack.secret);

			KeyValue *data = (KeyValue *)searchNode(client.connected_group->keyValue_List, (Item)&data_to_find, compareKeys);

			if (data == NULL) {
				strcpy(pack.groupID, "declined");
				send(client.socket, (void *)&pack, sizeof(pack), 0);
			} else {
				// Como encontramos vamos enviar esta informacao
				strcpy(pack.groupID, "accepted");
				send(client.socket, (void *)&pack, sizeof(pack), 0);
			}

		} else if (n == 0) {
			printf("Nada\n");
			/*client.connected_group->connected = 0;

			// kvslib will send the app's PID, inside variable mode
			recv(client.socket, (void *)&pack, sizeof(pack), 0);*/
			// this app is closing so find the app in the apps list and set that to disconnected
			/*AppInfo *app_closing = (AppInfo *)calloc(1, sizeof(AppInfo));

			// app_closing->PID = pack.mode;
			app_closing->PID = new_app->PID;
			// basta enviar a struct com o PID preenchido pois a comparacao e feita tendo por base o PID apenas
			app_closing = (AppInfo *)searchNode(apps, (Item)app_closing, compareApps);

			if (app_closing == NULL) {
				printf("Algo de muito errado nao esta certo...\n");
			}

			// dar update da flag e colocar data em q desligou
			app_closing->end = time(NULL);
			app_closing->connected = 0;

			// atualizar na lista
			// envia-se no data to update e no proprio update a mm struct pois a comparacao e feita so com o PID
			apps = updateNode(apps, (Item)app_closing, (Item)app_closing, compareApps, freeApps);

			// finally set this tread as free
			client_thread_status[client.index_client_thread_status] = 0;  // POSSIVEL MUTEX???
			break;*/
		} else {
			printf("else\n");
			strcpy(pack.secret, "declined");
			send(client.socket, (void *)&pack, sizeof(pack), 0);
		}

		if ((client.connected_group->deleted == 1) && (client.connected_group->connected > 0)) {
			// isto vai ser posto do close connection
			// client.connected_group->connected -= 1;
			strcpy(pack.secret, "group-deleted");
			send(client.socket, (void *)&pack, sizeof(pack), 0);
		} else {
			printList(client.connected_group->keyValue_List, printKV, 0);
		}
	}
	if ((client.connected_group->deleted == 1) && (client.connected_group->connected == 0)) {
		// client.connected_group->keyValue_List = clearList(client.connected_group->keyValue_List, freeKeyValue);
		groupsList = deleteNode(groupsList, (Item)client.connected_group, compareGroups, freeGroup);
	}
	return NULL;
}
// pthread_equal(pthread_t t1, pthread_t t2);
int main() {
	srand(time(NULL));
	pthread_t client_threads[MAX_CONNECTIONS];

	Server_info_pack clients;

	setup_LocalServer(&clients);

	pthread_t serverUI_thread;
	pthread_create(&serverUI_thread, NULL, (void *)server_UI, (void *)&auth_server);

	listen(callbacks_server.socket, 10);
	listen(clients.socket, 10);
	printf("Waiting for connections!!\n");

	int newClient_socket = 0, n = 0, found = 0;
	Package pack;
	size_t size = 0;
	while (1) {
		// while (1) {
		newClient_socket = accept(clients.socket, NULL, NULL);
		if (newClient_socket != -1) {
			printf("Connected to %d\n", newClient_socket);
		}

		// Contruir o package que vai ser enviado para a thread que trata deste cliente
		Client_info newClient;
		newClient.socket = newClient_socket;
		// newClient.connected_group = (Grupo *)searchNode(groupsList, (Item)newGroup, compareGroups);
		found = 0;
		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (client_thread_status[i] == 0) {
				client_thread_status[i] = 1;
				newClient.index_client_thread_status = i;
				pthread_create(&client_threads[i], NULL, (void *)client_handler, (void *)&newClient);
				found = 1;
				break;
			}
		}

		if (found == 0) {
			close(newClient_socket);
			printf("Servidor está cheio tenta mais tarde.\n");
		}

		// }
	}

	close(clients.socket);
	unlink(SOCKNAME);
}