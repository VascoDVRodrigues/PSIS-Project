#include "KVS-lib.h"

#include "defs.h"

int local_server_sock;
struct sockaddr_un local_server;

int establish_connection(char *group_id, char *secret) {
	local_server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (local_server_sock < 0) {
		perror("Error creating socket");
		return -3;
	}
	local_server.sun_family = AF_UNIX;
	strcpy(local_server.sun_path, SOCKNAME);

	if (connect(local_server_sock, (struct sockaddr *)&local_server, sizeof(struct sockaddr_un)) < 0) {
		close(local_server_sock);
		perror("connecting stream socket");
		return -4;
	}
	// printf("Connected to local server\n");
	App_Package pack;
	strcpy(pack.value, group_id);
	strcpy(pack.key, secret);
	send(local_server_sock, (void *)&pack, sizeof(pack), 0);

	recv(local_server_sock, (void *)&pack, sizeof(pack), 0);
	if (strcmp(pack.key, "accepted") == 0) {
		// must save the client PID and send it to server
		printf("pid: %d\n", getpid());
		pack.mode = getpid();
		send(local_server_sock, (void *)&pack, sizeof(pack), 0);
		return 0;
	} else if (strcmp(pack.key, "accepted-group") == 0) {
		return -1;	// grupo certo, chave errada
	} else if (strcmp(pack.key, "declined-group") == 0) {
		// close(local_server_sock);
		return -2;	// grupo errado
	}
}

int put_value(char *key, char *value) {
	App_Package a;
	a.mode = 1;
	strcpy(a.key, key);
	strcpy(a.value, value);

	// printf("%s %s %d\n", a.key, a.value, a.mode);

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
		return -2;
	}
	if (recv(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("recieving response");
		return -3;
	}
	if (strcmp(a.key, "accepted") == 0) {
		return 0; 
	} else {
		return -1; //error saving value
	}
}
int get_value(char *key, char **value) {
	App_Package a;
	a.mode = 0;
	strcpy(a.key, key);

	// printf("%s %d\n", a.key, a.mode);

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
		return -2;
	}

	if (recv(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("Receiving message from server!!\n");
		return -3;
	}

	if (strcmp(a.key, "accepted") == 0) {
		*value = (char *)malloc(strlen(a.value) + 1);
		strcpy(*value, a.value);
		return 0;
	}
	return -1;
	// printf("Value: %s\tKey: %s\n", a.value, a.key);
}
int delete_value(char *key) {
	App_Package a;
	a.mode = 2;
	strcpy(a.key, key);
	strcpy(a.value, "empty");

	// printf("%s %d\n", a.key, a.mode);

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
		return -2;
	}

	int n = recv(local_server_sock, (void *)&a, sizeof(a), 0);
	if (n < 0) {
		perror("Receiving message from server!!\n");
		return -3;
	}
	if (strcmp(a.key, "accepted") == 0) {
		return 0;
	}
	return -1;
}
int register_callback(char *key, void (*callback_function)(char *)) {}
int close_connection() {
	/*Package a;
	a.mode = 3;

	strcpy(a.key, "empty");
	strcpy(a.value, "empty");

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
	}*/
	close(local_server_sock);
	return 0;
}