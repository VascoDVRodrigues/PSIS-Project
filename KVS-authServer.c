#include "defs.h"

typedef struct _auth_grupo {
	char groupID[1024];
	char secret[1024];
} Auth_grupo;

// Aqui basta uma lista simples, pq so estamos a guardar 2 strings
LinkedList *groups_list;

void freeGroup(Item item) { return; }

void printGroups(Item item) {
	Auth_grupo data = *(Auth_grupo *)item;
	printf("GroupID: %s\n", data.groupID);
	printf("Secret: %s\n", data.secret);
	return;
}

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
	char buf[32];
	Auth_Package pack;

	groups_list = createList();

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

	Auth_grupo *receivedData;

	while (1) {
		printList(groups_list, printGroups, 0);
		printf("--------------------\n");

		if (recvfrom(fd, &pack, sizeof(pack), 0, (struct sockaddr *)&client, &client_address_size) < 0) {
			perror("recvfrom()");
			exit(4);
		}

		receivedData = calloc(1, sizeof(Auth_grupo));
		strcpy(receivedData->groupID, pack.groupID);
		strcpy(receivedData->secret, pack.secret);

		// printf("Received: \n\tgroupID:%s\n\tsecret:%s\n\tmode:%d\n", pack.groupID, pack.secret, pack.mode);

		if (pack.mode == 1) {  // a new group has been created, save the info
			// Checking if the groups exists
			if (searchNode(groups_list, receivedData, compareGroups) != NULL) {	 // O grupo ja existe
				// Send a response sayng that it went NOT ok
				strcpy(pack.groupID, "group-already-exists");
				strcpy(pack.secret, "group-already-exists");
			} else {
				// If it doesnt exist, create
				groups_list = insertNode(groups_list, receivedData);
				// Send a response sayng that it went ok
				strcpy(pack.groupID, "group-saved");
				strcpy(pack.secret, "group-saved");
			}
		} else if (pack.mode == 2) {
			groups_list = deleteNode(groups_list, receivedData, compareGroups, freeGroup);
			// Send a response sayng that it went ok
			strcpy(pack.groupID, "group-deleted");
			strcpy(pack.secret, "group-deleted");
		} else if (pack.mode == 3) {  // show group info
			Auth_grupo *aux = (Auth_grupo *)searchNode(groups_list, receivedData, compareGroups);
			if (aux == NULL) {	// aka the group doesnt exist
				strcpy(pack.groupID, "group-doesnt-exist");
				strcpy(pack.secret, "group-doesnt-exist");
			} else {  // the group exists
				strcpy(pack.secret, aux->secret);
			}
		} else if (pack.mode == 10) {  // Client trying to authenticate
			Auth_grupo *aux = (Auth_grupo *)searchNode(groups_list, receivedData, compareGroups);
			if (aux == NULL) {	// aka the group doesnt exist
				strcpy(pack.groupID, "declined-group");
				strcpy(pack.secret, "declined-group");
			} else {  // the group exists
				// Check if password is correct
				if (strcmp(pack.secret, aux->secret) != 0) {  // incorrect password
					strcpy(pack.groupID, "accepted-group");
					strcpy(pack.secret, "declined-secret");
				} else {
					strcpy(pack.groupID, "accepted-group");
					strcpy(pack.secret, "accepted-secret");
					pack.mode = 1;	// authentication was valid
				}
			}
		}

		if (sendto(fd, &pack, sizeof(pack), 0, (struct sockaddr *)&client, client_address_size) < 0) {
			perror("Sending reply to client.. :(\n");
		}
	}

	close(fd);

	return 0;
}