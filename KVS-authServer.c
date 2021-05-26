#include "defs.h"

// Aqui basta uma lista simples, pq so estamos a guardar 2 strings
SubNode *groups_list;

int main() {
	pthread_t local_servers[MAX_CONNECTIONS];
	struct sockaddr_in server, client;
	int fd, client_address_size;
	char buf[32];
	Auth_Package pack;

	groups_list = create_LinkedList();

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
		printList(groups_list);
		printf("--------------------\n");

		if (recvfrom(fd, &pack, sizeof(pack), 0, (struct sockaddr *)&client, &client_address_size) < 0) {
			perror("recvfrom()");
			exit(4);
		}

		// printf("Received: \n\tgroupID:%s\n\tsecret:%s\n\tmode:%d\n", pack.groupID, pack.secret, pack.mode);

		if (pack.mode == 1) {  // a new group has been created, save the info
			// Checking if the groups exists
			if (searchNode(pack.groupID, groups_list) != NULL) {  // O grupo ja existe
				// Send a response sayng that it went NOT ok
				strcpy(pack.groupID, "group-already-exists");
				strcpy(pack.secret, "group-already-exists");
			} else {
				// If it doesnt exist, create
				groups_list = insertNode(pack.groupID, pack.secret, groups_list);
				// Send a response sayng that it went ok
				strcpy(pack.groupID, "group-saved");
				strcpy(pack.secret, "group-saved");
			}
		} else if (pack.mode == 2) {
			groups_list = deleteNode(pack.groupID, groups_list);
			// Send a response sayng that it went ok
			strcpy(pack.groupID, "group-deleted");
			strcpy(pack.secret, "group-deleted");
		} else if (pack.mode == 3) {  // show group info
			SubNode *aux = searchNode(pack.groupID, groups_list);
			if (aux == NULL) {	// aka the group doesnt exist
				strcpy(pack.groupID, "group-doesnt-exist");
				strcpy(pack.secret, "group-doesnt-exist");
			} else {  // the group exists
				strcpy(pack.secret, aux->value);
			}
		} else if (pack.mode == 10) {  // Client trying to authenticate
			SubNode *aux = searchNode(pack.groupID, groups_list);
			if (aux == NULL) {	// aka the group doesnt exist
				strcpy(pack.groupID, "declined-group");
				strcpy(pack.secret, "declined-group");
			} else {  // the group exists
				// Check if password is correct
				if (strcmp(pack.secret, aux->value) != 0) {	 // incorrect password
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