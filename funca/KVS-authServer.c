#include "defs.h"

// Struct to save information about a group
typedef struct _auth_grupo {
	char groupID[1024];
	char secret[1024];
} Auth_grupo;

// Linked List to save the groups data
LinkedList *groups_list;

void freeGroup(Item item) { return; }

/*
 * Function: printGroups
 * ----------------------------
 *   Prints the information inside de group that are saved on the linked list
 *
 *   item: pointer to group to print
 *
 *   returns: Nothing
 *
 */
void printGroups(Item item) {
	Auth_grupo data = *(Auth_grupo *)item;
	printf(BOLDGREEN "GroupID " RESET);
	printf(BOLDWHITE "%s \n" RESET, data.groupID);
	printf(BOLDGREEN "Secret " RESET);
	printf(BOLDWHITE "%s \n" RESET, data.secret);
	return;
}

/*
 * Function: compareGroups
 * ----------------------------
 *   Compares 2 groups that are saved on the linked list
 *
 *   g1: Group 1 to compare
 *   g2: Group 2 to compare
 *
 *   returns: 0 if the groups are equal
 * 		      1 otherwise
 *
 */
int compareGroups(Item g1, Item g2) {
	Auth_grupo G1 = *(Auth_grupo *)g1;
	Auth_grupo G2 = *(Auth_grupo *)g2;

	if (strcmp(G1.groupID, G2.groupID) == 0) {
		return 1;
	}
	return 0;
}

int main() {
	pthread_t local_servers[MAX_CONNECTIONS];
	struct sockaddr_in server, client;
	int fd, client_address_size;
	Auth_grupo *receivedData;
	Package pack;

	// Initialize a linked list to save the groups info
	groups_list = createList();

	// Initialize the socket for comunication with LocalServers (DOMAIN:UNIX TYPE: STREAM)
	fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd < 0) {
		printf("Error while creating socket\n");
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(8080);
	server.sin_addr.s_addr = INADDR_ANY;

	// Binds to the socket
	if (bind(fd, (const struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Couldn't bind to the port\n");
		return -1;
	}

	client_address_size = sizeof(client);

	while (1) {
		system("clear");
		printf(BOLDBLUE "\t\tAuthentication Server 🌐\n" RESET);
		printList(groups_list, printGroups, 0);

		// we must use recvfrom and not recv so that we can reply to the exact
		// client that sent the request
		if (recvfrom(fd, &pack, sizeof(pack), 0, (struct sockaddr *)&client, &client_address_size) < 0) {
			perror("recvfrom()");
			exit(4);
		}

		// allocate memory for the new data
		receivedData = calloc(1, sizeof(Auth_grupo));
		strcpy(receivedData->groupID, pack.groupID);
		strcpy(receivedData->secret, pack.secret);

		if (pack.mode == 1) {  // Request to create a new group
			// Checking if the groups exists
			if (searchNode(groups_list, receivedData, compareGroups) != NULL) {	 // Grup with this groupID already exists
				// Build the package that will be sent to client
				strcpy(pack.groupID, "group-already-exists");
				strcpy(pack.secret, "group-already-exists");
			} else {
				// If it doesn't already exist, create
				groups_list = insertNode(groups_list, receivedData);

				// Build the package that will be sent to client
				strcpy(pack.groupID, "group-saved");
				strcpy(pack.secret, "group-saved");
			}
		} else if (pack.mode == 2) {  // Request to delete a group
			groups_list = deleteNode(groups_list, receivedData, compareGroups, freeGroup);

			strcpy(pack.groupID, "group-deleted");
			strcpy(pack.secret, "group-deleted");
		} else if (pack.mode == 3) {  // Request show group info
			// Search for said group
			Auth_grupo *aux = (Auth_grupo *)searchNode(groups_list, receivedData, compareGroups);
			if (aux == NULL) {	// The group doesn't exist
				strcpy(pack.groupID, "group-doesnt-exist");
				strcpy(pack.secret, "group-doesnt-exist");
			} else {  // the group exists
				strcpy(pack.secret, aux->secret);
			}
		} else if (pack.mode == 10) {  // Client trying to authenticate
			// Search for the requested group
			Auth_grupo *aux = (Auth_grupo *)searchNode(groups_list, receivedData, compareGroups);
			if (aux == NULL) {	// The group doesn't exist
				strcpy(pack.groupID, "declined-group");
				strcpy(pack.secret, "declined-group");
			} else {  // the group exists
				// Check if password is correct
				if (strcmp(pack.secret, aux->secret) != 0) {  // incorrect password
					strcpy(pack.groupID, "accepted-group");
					strcpy(pack.secret, "declined-secret");
				} else {
					pack.mode = 1;	// authentication was valid
				}
			}
		}
		// Send a reply to the client with the package that was previously built
		if (sendto(fd, &pack, sizeof(pack), 0, (struct sockaddr *)&client, client_address_size) < 0) {
			perror("Sending reply to client.. :(\n");
		}
	}

	close(fd);

	return 0;
}