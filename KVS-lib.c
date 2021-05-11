#include "KVS-lib.h"

#define SOCKNAME "/tmp/socket1"
#define MESSAGE_SIZE 100

int local_server_sock;
struct sockaddr_un local_server;

typedef struct package {
	int mode;
	char key[MESSAGE_SIZE];
	char value[MESSAGE_SIZE];
} Package;

int establish_connection(char *group_id, char *secret) {
	local_server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (local_server_sock < 0) {
		perror("Error creating socket");
		return -1;
	}
	local_server.sun_family = AF_UNIX;
	strcpy(local_server.sun_path, SOCKNAME);

	if (connect(local_server_sock, (struct sockaddr *)&local_server, sizeof(struct sockaddr_un)) < 0) {
		close(local_server_sock);
		perror("connecting stream socket");
		return -2;
	}
	printf("Connected to local server\n");
	return 0;
}

int put_value(char *key, char *value) {
	Package a;
	a.mode = 1;
	strcpy(a.key, key);
	strcpy(a.value, value);

	// printf("%s %s %d\n", a.key, a.value, a.mode);

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
	}
	if (recv(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("recieving response");
	}
	if (strcmp(a.key, "accepted") == 0) {
		return 0;
	} else {
		return 1;
	}
}
int get_value(char *key, char **value) {
	Package a;
	a.mode = 0;
	strcpy(a.key, key);

	printf("%s %d\n", a.key, a.mode);

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
	}

	int n = recv(local_server_sock, (void *)&a, sizeof(a), 0);
	if (n < 0) {
		perror("Receiving message from server!!\n");
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
	Package a;
	a.mode = 2;
	strcpy(a.key, key);

	// printf("%s %d\n", a.key, a.mode);

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
	}

	int n = recv(local_server_sock, (void *)&a, sizeof(a), 0);
	if (n < 0) {
		perror("Receiving message from server!!\n");
	}
	if (strcmp(a.key, "accepted") == 0) {
		return 0;
	}
	return -1;
}
int register_callback(char *key, void (*callback_function)(char *)) {}
int close_connection() { printf("Hello World\n"); }