#include "KVS-lib.h"

#include "defs.h"

int local_server_sock;

struct sockaddr_un local_server;

struct sockaddr_un local_server_callbacks;

LinkedList *watchList;

pthread_t callback_th;

// Struct to save info regarding a callback
typedef struct {
	char key[128];						// key to monitor
	void (*callback_function)(char *);	// function that must run if yhe key is altered
} Callback_info;

/*
 * Function: compareCallbacks
 * ----------------------------
 *   Compares information regarding 2 callbacks.
 * 	 When this function is called the 1st argument is a Package
 *
 *   returns: 1 if the callbacks refer to the same key
 *			  0 otherwise
 */
int compareCallbacks(Item c1, Item c2) {
	Package C1 = *(Package *)c1;
	Callback_info C2 = *(Callback_info *)c2;

	if (strcmp(C1.secret, C2.key) == 0) {
		return 1;
	}
	return 0;
}

/*
 * Function: callback_handler
 * ----------------------------
 *   Thread that monitors the messages received by the aplications regarding keys updates
 *
 *   returns: void
 */
void *callback_handler() {
	int callbacks_sock;

	// creates socket descriptor to connecto to the local server socket dedicated to managing callbacks
	// (DOMAIN:UNIX TYPE: STREAM)
	callbacks_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (callbacks_sock < 0) {
		// perror("Error creating socket");
		return NULL;
	}
	local_server_callbacks.sun_family = AF_UNIX;
	strcpy(local_server_callbacks.sun_path, CALLBACKS_SOCK);

	if (connect(callbacks_sock, (struct sockaddr *)&local_server_callbacks, sizeof(struct sockaddr_un)) < 0) {
		close(callbacks_sock);
		// perror("connecting stream socket");
		return NULL;
	}
	Package pack;
	Callback_info *updated;

	int a;
	while (1) {
		// waits for a message from ther server
		a = recv(callbacks_sock, &pack, sizeof(pack), 0);
		if (a <= 0) {
			pthread_exit((void *)1);
		}

		// search for the key that was altered in the server in the watchlist
		updated = (Callback_info *)searchNode(watchList, (Item)&pack, compareCallbacks);
		if (updated != NULL) {	// aka the key was registered here
			// run the registered function
			updated->callback_function(updated->key);
		}  // else do nothing
	}
}

/*
 * Function: establish_connection
 * ----------------------------
 *   Connects the app to the local server
 *
 *   group_id: id of the group trying to connect (MAX SIZE 1024)
 *   secret: secret of the group trying to connect (MAX SIZE 1024)
 *
 *   returns: 0 if the connection was successfully made
 * 		     -1 the group existes but the secret isn't correct
 * 			 -2 the group doesn't exist
 * 			 -3 error in the connection to the server
 * 			 -4 timed out error
 * 		     -5 Full server
 */
int establish_connection(char *group_id, char *secret) {
	Package pack;

	// creates a watchlist to save the callback values and functions
	watchList = createList();

	// creates socket descriptor to connecto to the local server(DOMAIN:UNIX TYPE: STREAM)
	local_server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (local_server_sock < 0) {
		// perror("Error creating socket");
		return -3;
	}
	local_server.sun_family = AF_UNIX;
	strcpy(local_server.sun_path, SOCKNAME);

	if (connect(local_server_sock, (struct sockaddr *)&local_server, sizeof(struct sockaddr_un)) < 0) {
		close(local_server_sock);
		// perror("connecting stream socket");
		return -3;
	}

	strcpy(pack.groupID, group_id);
	strcpy(pack.secret, secret);

	// sends package containing the group_id and secret to the local server to be validated
	send(local_server_sock, (void *)&pack, sizeof(pack), 0);

	// recieves response regarding the status of the connection
	int a = recv(local_server_sock, (void *)&pack, sizeof(pack), 0);

	if (strcmp(pack.secret, "accepted") == 0) {
		// must save the client PID and send it to server
		pack.mode = getpid();
		send(local_server_sock, (void *)&pack, sizeof(pack), 0);

		// create a thread to manage the callback process
		pthread_create(&callback_th, NULL, (void *)callback_handler, NULL);
		return 0;
	} else if (strcmp(pack.groupID, "accepted-group") == 0) {
		// the group existes but the secret isn't correct
		return -1;
	} else if (strcmp(pack.secret, "declined-group") == 0) {
		// the group doesn't exist
		return -2;
	} else if (strcmp(pack.secret, "timed-out") == 0) {
		// the local server didn't respond on a set amount of time
		return -4;
	} else if (a <= 0) {
		// the local server didn't respond at all
		return -4;
	}
}

/*
 * Function: put_value
 * ----------------------------
 *   Given a key and a value, it stores those values on the connected group
 *
 *   key: a key to identify the stored value (MAX SIZE 128)
 *   value: the value to store (NO MAX SIZE)
 *
 *   returns: 0 the value was successfully stored on the local server
 * 		     -1 there was an error saving the value on the local server
 * 			 -2 the group doesnt exist. Connection will be automatically closed
 * 			 -3 there was an error comunicating with the server
 */
int put_value(char *key, char *value) {
	Package a;
	a.mode = 1;
	char key_to_send[128];
	size_t size;

	// to garantee that the key has a max size of 128
	strncpy(key_to_send, key, 127);
	key_to_send[127] = '\0';

	strcpy(a.secret, key_to_send);

	// Send the size of the value
	size = strlen(value) + 1;
	sprintf(a.groupID, "%ld", size);

	// Send the mode, the key and the size of the value
	if (send(local_server_sock, (void *)&a, sizeof(a), 0) <= 0) {
		// there was an error sending the data to the socket
		// perror("writing on stream socket");
		return -3;
	}

	// send the value to store on the local server
	if (send(local_server_sock, (void *)value, size, 0) < 0) {
		// perror("sending value to store");
		return -3;
	}

	// Recieve the response from server
	if (recv(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		// perror("recieving response");
		return -3;
	}
	// The value was correctly stored on the local server
	if (strcmp(a.secret, "accepted") == 0) {
		return 0;
	} else if (strcmp(a.secret, "group-deleted") == 0) {
		// the group doesnt exist. Must close the connection
		close_connection();
		return -2;
	} else {
		// there was an error saving the value on the local server
		return -1;
	}
}

/*
 * Function: get_value
 * ----------------------------
 *   Given a key and a pointer to a variable, it stores the value of said key on the connected group to the variable pointed by the pointer
 *
 *   key: a key to identify the value to retrieve
 *   value: the pointer to the variable in which the value of the key will be stored
 *
 *   returns: 0 the value was successfully retrieved from the local server
 * 		     -1 there was an error retrieving the value from the local server
 * 			 -2 the group doesnt exist. Connection will be automatically closed
 *			 -3 there was an error comunicating with the server
 *
 */
int get_value(char *key, char **value) {
	Package a;
	a.mode = 0;
	size_t size = 0;
	char key_to_send[128];

	// garantees the key will have 128 characters
	strncpy(key_to_send, key, 127);
	key_to_send[127] = '\0';

	strcpy(a.secret, key_to_send);

	// Send the mode and key to the socket
	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		// perror("writing on stream socket");
		return -3;
	}

	// local server responds
	if (recv(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		// perror("Receiving message from server!!\n");
		return -3;
	}

	if (strcmp(a.secret, "accepted") == 0) {
		// if the value exists on the local server, it will send the size of the value to alocate the memory
		if (recv(local_server_sock, (void *)&size, sizeof(size), 0) < 0) {
			return -3;
		}
		*value = (char *)malloc(sizeof(char) * size);
		// then the local server sends the value to be stored on the value
		if (recv(local_server_sock, (void *)*value, size, 0) < 0) {
			return -3;
		}
		return 0;
	} else if (strcmp(a.secret, "group-deleted") == 0) {
		// the group doesn't exist anymore. Must close the connection
		close_connection();
		return -4;
	}
	// there was an error retrieving the value from the local server
	return -1;
}

/*
 * Function: delete_value
 * ----------------------------
 *   Given a key it deletes the value of said key on the connected group
 *
 *   key: a key to identify the value to delete
 *
 *   returns: 0 the value was successfully retrieved from the local server
 * 		     -1 there was an error deleting the value from the local server
 *			 -2 the group doesnt exist. Connection will be automatically closed
 *			 -3 there was an error comunicating with the server
 *
 */
int delete_value(char *key) {
	Package a;
	a.mode = 2;

	char key_to_send[128];

	strncpy(key_to_send, key, 127);
	key_to_send[127] = '\0';  // ver os comentarios do put_value

	strcpy(a.secret, key_to_send);

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		// perror("writing on stream socket");
		return -3;
	}

	int n = recv(local_server_sock, (void *)&a, sizeof(a), 0);
	if (n < 0) {
		// perror("Receiving message from server!!\n");
		return -3;
	}
	if (strcmp(a.secret, "accepted") == 0) {
		return 0;
	} else if (strcmp(a.secret, "group-deleted") == 0) {
		close_connection();
		return -2;
	}
	return -1;
}

/*
 * Function: printWatch
 * ----------------------------
 * Prints information about the registered callback stored in the watchList
 *
 */
void printWatch(Item c1) {
	Callback_info current = *(Callback_info *)c1;
	printf("Key: %s||func: %p\n", current.key, current.callback_function);
}

/*
 * Function: register_callback
 * ----------------------------
 *   Given a key it registeres a function to be executed whenever the value of that key is altered
 *
 *   key: a key to identify the value whose update triggers the callback
 *   (*callback_function)(char*): pointer to function that MUST receive a char* and return void
 *
 *   returns: 0 the callback was successfully implemented
 *			 -1 error registering the callback
 *
 */
int register_callback(char *key, void (*callback_function)(char *)) {
	// Creates node of the linked list that stores the callback data
	Callback_info *newData = calloc(1, sizeof(Callback_info));
	if (newData == NULL) {
		return -1;
	}

	// Garantees that the key doesnt exceed the max size
	char key_to_save[128];
	strncpy(key_to_save, key, 127);
	key_to_save[127] = '\0';

	strcpy(newData->key, key_to_save);
	newData->callback_function = callback_function;

	// inserts the node on the watchlist
	watchList = insertNode(watchList, (Item)newData);

	return 0;
}

/*
 * Function: close_connection
 * ----------------------------
 *   Closes the connection between a client and the local server
 *
 *   returns: 0 the connection was successfully closed
 *			 -1 there was an error comunicating with the server
 *
 */

int close_connection() {
	Package a;
	a.mode = 3;

	// just send a package with the mode 3
	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
		return -1;
	}

	if (close(local_server_sock) != 0) {
		return -1;
	}
	return 0;
}