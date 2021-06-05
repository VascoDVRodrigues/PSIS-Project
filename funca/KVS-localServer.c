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

pthread_rwlock_t Group_acess, Apps_acess, callback_lock, threads_lock;

/*
 * Function: printKV
 * ----------------------------
 *   Prints the information about a key|value that are saved on the linked list
 *
 *   item: pointer to Key|value to print
 *
 */
void printKV(Item item) {
	KeyValue data = *(KeyValue *)item;
	printf("%s|%s\n", data.key, data.value);
	return;
}

/*
 * Function: freeKeyValue
 * ----------------------------
 *   Frees a Key|value that are saved on the linked list
 *
 *   item: pointer to Key|value to delete
 *
 */
void freeKeyValue(Item item) {
	KeyValue *KV = (KeyValue *)item;
	free(KV->value);
	free(KV);
	return;
}

/*
 * Function: freeGroup
 * ----------------------------
 *   Frees a group that are saved on the linked list
 *
 *   item: pointer to group to delete
 *
 */
void freeGroup(Item item) {
	Grupo *g = (Grupo *)item;
	g->keyValue_List = clearList(g->keyValue_List, freeKeyValue);
	free(g);
}

/*
 * Function: compareGroups
 * ----------------------------
 *   Compares 2 groups that are saved on the linked list
 *
 *   g1: Group 1 to compare
 *   g2: Group 2 to compare
 *
 *   returns: 1 if the groups are equal
 * 		      0 otherwise
 *
 */
int compareGroups(Item g1, Item g2) {
	Grupo data1 = *(Grupo *)g1;
	Grupo data2 = *(Grupo *)g2;

	if (strcmp(data1.groupID, data2.groupID) == 0) {
		return 1;
	}
}

/*
 * Function: compareKeys
 * ----------------------------
 *   Compares 2 keys that are saved on the linked list
 *
 *   k1: Key 1 to compare
 *   k2: Key 2 to compare
 *
 *   returns: 1 if the keys are equal
 * 		      0 otherwise
 *
 */
int compareKeys(Item k1, Item k2) {
	KeyValue data1 = *(KeyValue *)k1;
	KeyValue data2 = *(KeyValue *)k2;

	if (strcmp(data1.key, data2.key) == 0) {
		return 1;
	}

	return 0;
}

/*
 * Function: compareApps
 * ----------------------------
 *   Compares 2 apps that are saved on the linked list
 *
 *   a1: app 1 to compare
 *   a2: app 2 to compare
 *
 *   returns: 1 if the apps are equal
 * 		      0 otherwise
 *
 */
int compareApps(Item a1, Item a2) {
	AppInfo A1 = *(AppInfo *)a1;
	AppInfo A2 = *(AppInfo *)a2;

	if (A1.PID == A2.PID) {
		return 1;
	}
	return 0;
}

/*
 * Function: printApp
 * ----------------------------
 *   Prints the information inside the app that are saved on the linked list
 *
 *   item: pointer to app to print
 *
 */
void printApp(Item a) {
	AppInfo A = *(AppInfo *)a;

	// Dont use ctime_r() inside the printf or ctime at all, check man page for details on thread safety
	// Check man page of ctime_r to understand why start and end have size 26
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
		printf("disconnected.\n Started connection at %s Ended connection at %s", start, end);
	}
	return;
}

void freeApps(Item a1) {
	// free(a1);
	return;
}

/*
 * Function: randomString
 * ----------------------------
 *   Generates a random string with length size
 *
 *   size: length of the string to generate
 *
 *   returns: Generated string
 *
 */
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
 * Function: setup_LocalServer
 * ----------------------------
 *   Creates and binds socket to comunicate with clients
 * 	 Creates socket to comunicate with authServer
 * 	 Creates socket to comunicate callbacks
 * 	 Initializes all rw_locks
 *	 Initializes all lists (groups and apps)
 *
 *   clients: struct that saves info about the clients that will connect to this server
 *            Must be of the type Server_info_pack
 *
 *   returns: 0: no errors
 *			 -1: Error creating stream socket
 *			 -2: Error binding stream socket
 *			 -3: Error Auth socket creation
 *
 */
int setup_LocalServer(Server_info_pack *clients) {
	// Initializes lists
	groupsList = createList();
	apps = createList();

	// Initializes all rw_locks
	pthread_rwlock_init(&Group_acess, NULL);
	pthread_rwlock_init(&Apps_acess, NULL);
	pthread_rwlock_init(&callback_lock, NULL);
	pthread_rwlock_init(&threads_lock, NULL);

	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		client_thread_status[i] = 0;
		callbackSockets[i] = 0;
	}

	// Creates and binds socket to comunicate with clients
	// client <-> localserver
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

	// Creates socket to comunicate with authServer
	// localserver <-> authserver
	if ((auth_server.socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Auth socket creation");
		return -3;
	}

	auth_server.adress.sin_family = AF_INET;
	auth_server.adress.sin_port = htons(8080);
	inet_aton(AUTHSERVER_IP, &auth_server.adress.sin_addr);

	// set a timeout for the auth server
	// in future connections if the server doesn't respond in AUTHSERVER_TIMEOUT seconds
	// an error will be return
	struct timeval timeout;
	timeout.tv_sec = AUTHSERVER_TIMEOUT;
	timeout.tv_usec = 0;

	// man setsockopt e man 7 socket to understand this flags
	setsockopt(auth_server.socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);

	// Creates socket to comunicate callbacks
	// local-server <-> kvs-lib callback
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

	return 0;
}

/*
 * Function: server_UI
 * ----------------------------
 *   Implements the local server user interface
 * 	 Runs as a separated thread
 *
 *   arg: pointer to struct with info about auth server
 *
 */
void *server_UI(void *arg) {
	Server_info_pack auth_server = *(Server_info_pack *)arg;

	int option = 0;
	int err = 0;
	Package pack;
	while (1) {
		printf(BOLDBLUE "\n\tKVS Local Server 💻\n");
		printf(BOLDWHITE "1. Create Group\n2. Delete Group\n3. Show group info\n");
		printf("4. Show app status\n5. Exit\n");
		printf("\tChoose one option: ");
		scanf("%d", &option);
		if (option == 1) {	// create group
			printf("Group identifier: ");
			scanf("%s", pack.groupID);

			// generate a new random string to be the secret
			strcpy(pack.secret, randomString(5));

			// Uncomment this to simulate the improbable case of someone trying to connect with the correct password
			// of the group that is being created
			// strcpy(pack.secret, "known_secret");

			pack.mode = 1;

			// block write to all groups
			pthread_rwlock_wrlock(&Group_acess);

			// Send request to auth server
			sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

			err = recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
			if (err < 0) {
				printf("Timed out, try again later please ⌛️\n");
			} else {
				printf("Secret: %s\n", pack.secret);
			}
			// unblock the groups
			pthread_rwlock_unlock(&Group_acess);
		} else if (option == 2) {  // Delete group
			printf("Group ID to delete: ");
			scanf("%s", pack.groupID);

			pack.mode = 2;
			Grupo G_to_delete;
			strcpy(G_to_delete.groupID, pack.groupID);

			// block write acess to all groups
			pthread_rwlock_wrlock(&Group_acess);

			// Send request to server to delete the group
			sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

			err = recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
			if (err < 0) {
				printf("Timed out, try again later please ⌛️\n");
				pthread_rwlock_unlock(&Group_acess);
			} else {
				// if theres no error delete from the list
				Grupo *g_delete;
				g_delete = (Grupo *)searchNode(groupsList, (Item)&G_to_delete, compareGroups);
				if (g_delete != NULL) {
					groupsList = deleteNode(groupsList, (Item)&G_to_delete, compareGroups, freeGroup);
				}
				pthread_rwlock_unlock(&Group_acess);
			}
		} else if (option == 3) {  // Show group info
			printf("Group ID to view info: ");
			scanf("%s", pack.groupID);
			pack.mode = 3;

			Grupo grupo_to_find;
			strcpy(grupo_to_find.groupID, pack.groupID);

			// Send request to auth server
			sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

			err = recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
			if (err < 0) {
				printf("Timed out, try again later please ⌛️\n");
			} else {
				if (strcmp(pack.groupID, "group-doesnt-exist") == 0) {
					printf("This group doesn't exist ❌\n");
				} else {
					// Block all groups for reading
					pthread_rwlock_rdlock(&Group_acess);
					// search this group in the groups list to display #keys|values
					Grupo *grupo_found = (Grupo *)searchNode(groupsList, (Item)&grupo_to_find, compareGroups);

					if (grupo_found == NULL) {	// if the group exist on auth server but is not here locally
						pthread_rwlock_unlock(&Group_acess);
						printf("\tThis group doesn't exist here ❌\n");
					} else {  // display #keys|values
						printf("\tHas %d keys|values 🔑\n", grupo_found->n_keyValues);
						pthread_rwlock_unlock(&Group_acess);
					}
				}
			}
		} else if (option == 4) {  // Show app status

			// Block all apps for reading
			pthread_rwlock_rdlock(&Apps_acess);
			printList(apps, printApp, 0);
			pthread_rwlock_unlock(&Apps_acess);
		} else if (option == 5) {  // Exit
			// clear variables
			clearList(groupsList, freeGroup);
			clearList(apps, freeApps);
			for (int i = 0; i < MAX_CONNECTIONS; i++) {
				close(callbackSockets[i]);
			}
			close(auth_server.socket);

			// and destroy rw_locks
			pthread_rwlock_destroy(&Group_acess);
			pthread_rwlock_destroy(&Apps_acess);
			pthread_rwlock_destroy(&callback_lock);
			pthread_rwlock_destroy(&threads_lock);
			exit(1);
			break;
		}
	}
}

/*
 * Function: callback_handler
 * ----------------------------
 *   Comunicates to all clients that a certain key's value was altered
 * 	 This function runs in a separated thread
 *
 *   arg: string containing the key that was altered
 *
 */
void *callback_handler(void *arg) {
	char *alteredKey = (char *)arg;

	Package pack;
	strcpy(pack.secret, alteredKey);

	// lock the callback Sockets array for reading
	pthread_rwlock_rdlock(&callback_lock);
	for (int i = 0; i < MAX_CONNECTIONS; i++) {
		if (callbackSockets[i] != 0) {
			// send the same message to all clients
			send(callbackSockets[i], (void *)&pack, sizeof(pack), 0);
		}
	}
	pthread_rwlock_unlock(&callback_lock);
}

/*
 * Function: client_handler
 * ----------------------------
 *   Handles the client connected to the server (authentication and future operations)
 * 	 This function runs in a separated thread
 * 	 Each new client has a dedicated thread that runs this function
 *
 *   arg: struct with client information (must be of type Client_info)
 *
 */
void *client_handler(void *arg) {
	Client_info client = *(Client_info *)arg;
	int n = 0, x = 0, new_callback_sock;
	Package pack;
	AppInfo *new_app;
	pthread_t callback_th;

	// client sent the groupID and the secret
	n = recv(client.socket, (void *)&pack, sizeof(pack), 0);

	pthread_rwlock_wrlock(&Group_acess);  // Authentication starts here
	// Lock here all groups

	// sleep(10);
	// Package to send to the auth server is the same that was received, just change the mode
	pack.mode = 10;
	sendto(auth_server.socket, (void *)&pack, sizeof(pack), 0, (struct sockaddr *)&auth_server.adress, sizeof(auth_server.adress));

	// process response from auth server
	x = recv(auth_server.socket, (void *)&pack, sizeof(pack), 0);
	if (x < 0) {
		// the server didnt respond in time
		pack.mode = 0;
		strcpy(pack.groupID, "timed-out");
	}

	if (pack.mode == 1) {  // authentication was ok

		// Since the group exists we should add it to the groups list
		Grupo *newGroup = calloc(1, sizeof(Grupo));
		newGroup->keyValue_List = createList();	 // list to store keys|values
		newGroup->n_keyValues = 0;

		strcpy(newGroup->groupID, pack.groupID);

		// Check if it was already added to the list by other app
		if (searchNode(groupsList, (Item)newGroup, compareGroups) == NULL) {  
			//the group doesn't exist yet
			groupsList = insertNode(groupsList, (Item)newGroup);
		}

		// comunicate to client, inside secret
		strcpy(pack.secret, "accepted");
		send(client.socket, (void *)&pack, sizeof(pack), 0);

		// the kvslib will send the client PID, in pack.mode
		recv(client.socket, (void *)&pack, sizeof(pack), 0);

		// Save it in the apps list
		new_app = (AppInfo *)calloc(1, sizeof(AppInfo));
		new_app->connected = 1;
		new_app->PID = pack.mode;
		new_app->start = time(NULL);
		apps = insertNode(apps, (Item)new_app);

		// Save a pointer to the group that the client asked to connect
		client.connected_group = (Grupo *)searchNode(groupsList, (Item)newGroup, compareGroups);

		// Wait for a new connect, but on the socket created for the callbacks
		new_callback_sock = accept(callbacks_server.socket, NULL, NULL);

		// lock for writing the callbackSockets array
		pthread_rwlock_wrlock(&callback_lock);
		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (callbackSockets[i] == 0) {
				// save this client callback socket so in the future we can comunicate callbacks
				callbackSockets[i] = new_callback_sock;
				client.callback_sock_index = i;
				break;
			}
		}
		pthread_rwlock_unlock(&callback_lock);
		pthread_rwlock_unlock(&Group_acess);  // the authentication ends here and not before the attribution
											  // of the callback socket
	} else {
		// authentication was not ok
		pthread_rwlock_unlock(&Group_acess);  // no need to acess the groups list, nothing will be saved

		if (strcmp(pack.groupID, "accepted-group") == 0) {
			if (strcmp(pack.secret, "declined-secret") == 0) {	// correct groupID but wrong key
				strcpy(pack.groupID, "accepted-group");
				strcpy(pack.secret, "declined-secret");
			}
		} else if (strcmp(pack.groupID, "declined-group") == 0) {  // wrong groupID
			strcpy(pack.secret, "declined-group");
		} else if (strcmp(pack.groupID, "timed-out") == 0) {  // server timed out
			strcpy(pack.secret, "timed-out");
		}

		// comunicate to client
		send(client.socket, (void *)&pack, sizeof(pack), 0);

		// Kill this thread
		pthread_rwlock_wrlock(&threads_lock);
		client_thread_status[client.index_client_thread_status] = 0;
		pthread_rwlock_unlock(&threads_lock);
		pthread_exit((void *)1);
	}

	while (1) {	 // wait for client requests
		n = recv(client.socket, (void *)&pack, sizeof(pack), 0);

		if (pack.mode == 1 && n > 0) {	// PUT VALUE
			KeyValue *newData = calloc(1, sizeof(KeyValue));

			// Client sent key inside secret
			strcpy(newData->key, pack.secret);

			// Client sent the size of the value inside groupID
			size_t size = atoi(pack.groupID);

			// allocate memory to save the value with the received size
			newData->value = (char *)malloc(sizeof(char) * size);

			// and save the actual value
			recv(client.socket, (void *)newData->value, size, 0);

			// block all groups for writing
			pthread_rwlock_wrlock(&Group_acess);

			// update the pointer to the connected group
			client.connected_group = (Grupo *)searchNode(groupsList, (Item)client.connected_group, compareGroups);

			if (client.connected_group != NULL) {  // still exists
				// search for the key inside the key|value list of this group
				KeyValue *found = (KeyValue *)searchNode(client.connected_group->keyValue_List, newData, compareKeys);
				if (found == NULL) {
					// this key doesn't exist, save
					client.connected_group->keyValue_List = insertNode(client.connected_group->keyValue_List, (Item)newData);
					client.connected_group->n_keyValues += 1;
				} else {
					// the key exists
					// check if the update is what it was already being saved
					if (strcmp(found->value, newData->value) != 0) {
						// if not lets update the key
						client.connected_group->keyValue_List =
							updateNode(client.connected_group->keyValue_List, (Item)newData, (Item)newData, compareKeys, freeKeyValue);
						// the data_to_find and update arguments are the same variable because the comparation of 2 keys
						// is only based on the key name and not on the value that it saves

						// as the key was updated, notify all clients
						pthread_create(&callback_th, NULL, (void *)callback_handler, (void *)&newData->key);
					}  // else there was no actual update
				}
				pthread_rwlock_unlock(&Group_acess);  // unlock all groups
				strcpy(pack.secret, "accepted");
				send(client.socket, (void *)&pack, sizeof(pack), 0);
			} else {
				pthread_rwlock_unlock(&Group_acess);
			}

		} else if (pack.mode == 0 && n > 0) {  // GET VALUE
			KeyValue data_to_find;

			// client sent the key on pack.secret
			strcpy(data_to_find.key, pack.secret);

	 		// block all groups for reading
			pthread_rwlock_rdlock(&Group_acess); 
			// update the pointer to the connected group
			client.connected_group = (Grupo *)searchNode(groupsList, (Item)client.connected_group, compareGroups);
			if (client.connected_group != NULL) {  // group still exists
				// search for the key
				KeyValue *data = (KeyValue *)searchNode(client.connected_group->keyValue_List, (Item)&data_to_find, compareKeys);

				pthread_rwlock_unlock(&Group_acess);  // unlock all the groups

				if (data == NULL) {	 // key doesnt exist in this group
					strcpy(pack.secret, "declined");
					send(client.socket, (void *)&pack, sizeof(pack), 0);
				} else {  // key exists
					strcpy(pack.secret, "accepted");
					send(client.socket, (void *)&pack, sizeof(pack), 0);

					// send the size of the value and then the actual value
					size_t size = strlen(data->value) + 1;
					send(client.socket, (void *)&size, sizeof(size), 0);

					send(client.socket, (void *)data->value, size, 0);
				}

			} else {  
				pthread_rwlock_unlock(&Group_acess);
			}
		} else if (pack.mode == 2 && n > 0) {  // DELETE VALUE
			KeyValue data_to_delete;

			// client sent the key to delete in pack.secret
			strcpy(data_to_delete.key, pack.secret);

			pthread_rwlock_wrlock(&Group_acess);  // block all groups for reading
			// update the pointer to the connected group
			client.connected_group = (Grupo *)searchNode(groupsList, (Item)client.connected_group, compareGroups);
			if (client.connected_group != NULL) {  // group still exists
				// search for the key
				if (searchNode(client.connected_group->keyValue_List, (Item)&data_to_delete, compareKeys) == NULL) {
					// key doesn't exist in this group
					strcpy(pack.secret, "declined");
				} else {
					// key exists, delete it
					client.connected_group->keyValue_List =
						deleteNode(client.connected_group->keyValue_List, (Item)&data_to_delete, compareKeys, freeKeyValue);
					strcpy(pack.secret, "accepted");
				}
				send(client.socket, (void *)&pack, sizeof(pack), 0);
			}  // else do nothing
			pthread_rwlock_unlock(&Group_acess);
		} else if (pack.mode == 3 || n >= 0) {	// close connection
			// this app is closing so find the app in the apps list and set that to disconnected
			AppInfo *app_closing = (AppInfo *)calloc(1, sizeof(AppInfo));

			// app_closing->PID = pack.mode;
			app_closing->PID = new_app->PID;
			// the compare of apps is based on the pid only
			pthread_rwlock_wrlock(&Apps_acess);
			app_closing = (AppInfo *)searchNode(apps, (Item)app_closing, compareApps);

			// sanity check this pointer will never be NULL
			if (app_closing == NULL) {
				printf("Something is very wrong...\n");
			}

			// Update the status of the app and the time it ended connection
			app_closing->end = time(NULL);
			app_closing->connected = 0;

			// update apps list
			// the compare of apps is based on the pid only
			apps = updateNode(apps, (Item)app_closing, (Item)app_closing, compareApps, freeApps);

			pthread_rwlock_unlock(&Apps_acess);
			// finally set this tread as free
			pthread_rwlock_wrlock(&threads_lock);
			client_thread_status[client.index_client_thread_status] = 0;
			pthread_rwlock_unlock(&threads_lock);

			// also set this socket as free
			pthread_rwlock_wrlock(&callback_lock);
			callbackSockets[client.callback_sock_index] = 0;
			pthread_rwlock_unlock(&callback_lock);
			break;
		} else if (pack.mode == 4 && n > 0) {  // register callback
			KeyValue data_to_find;

			// client sent the key inside pack.secret
			strcpy(data_to_find.key, pack.secret);

			pthread_rwlock_rdlock(&Group_acess);

			// update the pointer to the connected group
			client.connected_group = (Grupo *)searchNode(groupsList, (Item)client.connected_group, compareGroups);

			if (client.connected_group != NULL) {  // still exists
				KeyValue *data = (KeyValue *)searchNode(client.connected_group->keyValue_List, (Item)&data_to_find, compareKeys);
				pthread_rwlock_unlock(&Group_acess);
				if (data == NULL) {	 // key doesn't exist
					strcpy(pack.groupID, "declined");
					send(client.socket, (void *)&pack, sizeof(pack), 0);
				} else {
					// Key exists
					strcpy(pack.groupID, "accepted");
					send(client.socket, (void *)&pack, sizeof(pack), 0);
				}
			} else {
				pthread_rwlock_unlock(&Group_acess);
			}

		} else {
			strcpy(pack.secret, "declined");
			send(client.socket, (void *)&pack, sizeof(pack), 0);
		}

		// if the client.connected group is null it means it was deleted
		if (client.connected_group == NULL) {
			strcpy(pack.secret, "group-deleted");
			send(client.socket, (void *)&pack, sizeof(pack), 0);
		}
	}
	return NULL;
}

int main() {
	srand(time(NULL));
	pthread_t client_threads[MAX_CONNECTIONS];

	Server_info_pack clients;

	int newClient_socket = 0, n = 0, found = 0;
	Package pack;
	size_t size = 0;

	setup_LocalServer(&clients);

	pthread_t serverUI_thread;
	pthread_create(&serverUI_thread, NULL, (void *)server_UI, (void *)&auth_server);

	listen(callbacks_server.socket, 10);
	listen(clients.socket, 10);

	while (1) {
		newClient_socket = accept(clients.socket, NULL, NULL);

		// build the package that will be sent to the thread that will handle this new client
		Client_info newClient;
		newClient.socket = newClient_socket;

		found = 0;
		pthread_rwlock_wrlock(&threads_lock);
		// check if #MAX_CONNECTIONS was reached
		for (int i = 0; i < MAX_CONNECTIONS; i++) {
			if (client_thread_status[i] == 0) {
				client_thread_status[i] = 1;
				newClient.index_client_thread_status = i;
				pthread_create(&client_threads[i], NULL, (void *)client_handler, (void *)&newClient);
				found = 1;
				break;
			}
		}
		pthread_rwlock_unlock(&threads_lock);
		if (found == 0) {
			close(newClient_socket);
			printf("The server is full 👥\n");
		}
	}
	close(clients.socket);
	unlink(SOCKNAME);
}