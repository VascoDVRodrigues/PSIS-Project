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
	Package pack;
	strcpy(pack.groupID, group_id);
	strcpy(pack.secret, secret);

	send(local_server_sock, (void *)&pack, sizeof(pack), 0);

	recv(local_server_sock, (void *)&pack, sizeof(pack), 0);
	if (strcmp(pack.secret, "accepted") == 0) {
		// must save the client PID and send it to server
		pack.mode = getpid();
		send(local_server_sock, (void *)&pack, sizeof(pack), 0);
		return 0;
	} else if (strcmp(pack.groupID, "accepted-group") == 0) {
		return -1;	// grupo certo, chave errada
	} else if (strcmp(pack.secret, "declined-group") == 0) {
		// close(local_server_sock);
		return -2;	// grupo errado
	}
}

int put_value(char *key, char *value) {
	printf("Value recebido na func: %s\n", value);
	// VALUE TEM UMA DIMENSAO "INFINITA"
	Package a;
	a.mode = 1;

	char key_to_send[128];
	strncpy(key_to_send, key, 127);	 // Guardar 1 byte para o \0
	key_to_send[127] = '\0';		 // para o caso de a key ser maior que 128, temos que por o \0
	// para ver porque ler a man page da funcao strncpy

	strcpy(a.secret, key_to_send);

	// enviar o modo e a key
	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
		return -2;
	}

	// Enviar o tamanho do value e depois o value
	size_t size = strlen(value) + 1;
	send(local_server_sock, (void *)&size, sizeof(size_t), 0);

	send(local_server_sock, (void *)value, size, 0);

	// Response from server
	if (recv(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("recieving response");
		return -3;
	}
	if (strcmp(a.secret, "accepted") == 0) {
		return 0;
	} else if (strcmp(a.secret, "group-deleted") == 0) {
		close_connection();
		return -4;
	} else {
		return -1;	// error saving value
	}
}

int get_value(char *key, char **value) {
	Package a;
	a.mode = 0;

	// Fazer como no put_value

	char key_to_send[128];
	strncpy(key_to_send, key, 127);
	key_to_send[127] = '\0';  // ver os comentarios do put_value

	strcpy(a.secret, key_to_send);

	// enviar o modo e a key
	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
		return -2;
	}

	// ver a resposta do server a esta key
	if (recv(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("Receiving message from server!!\n");
		return -3;
	}
	size_t size = 0;
	if (strcmp(a.secret, "accepted") == 0) {
		// O server vai enviar o tamanho do value e depois o value
		recv(local_server_sock, (void *)&size, sizeof(size), 0);
		*value = (char *)malloc(sizeof(char) * size);
		recv(local_server_sock, (void *)*value, size, 0);
		return 0;
	} else if (strcmp(a.secret, "group-deleted")) {
		close_connection();
		return -4;
	}

	return -1;
	// printf("Value: %s\tKey: %s\n", a.value, a.key);
}
int delete_value(char *key) {
	Package a;
	a.mode = 2;

	char key_to_send[128];
	strncpy(key_to_send, key, 127);
	key_to_send[127] = '\0';  // ver os comentarios do put_value

	strcpy(a.secret, key_to_send);

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
		return -2;
	}

	int n = recv(local_server_sock, (void *)&a, sizeof(a), 0);
	if (n < 0) {
		perror("Receiving message from server!!\n");
		return -3;
	}
	if (strcmp(a.secret, "accepted") == 0) {
		return 0;
	}
	return -1;
}
int register_callback(char *key, void (*callback_function)(char *)) {
	Package a;
	a.mode = 4;

	char key_to_send[128];
	strncpy(key_to_send, key, 127);
	key_to_send[127] = '\0';  // ver os comentarios do put_value

	strcpy(a.secret, key_to_send);

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
	}

	// ver a resposta do server a esta key
	if (recv(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("Receiving message from server!!\n");
		return -3;
	}
	size_t size = 0;
	if (strcmp(a.groupID, "accepted") == 0) {
		return 0;
	}

	// Enviar agr o ponteiro para a func
}

int close_connection() {
	Package a;
	a.mode = 3;

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
	}

	// send this app's PID to local server inside variable "mode"

	a.mode = getpid();

	if (send(local_server_sock, (void *)&a, sizeof(a), 0) < 0) {
		perror("writing on stream socket");
	}

	close(local_server_sock);
	return 0;
}