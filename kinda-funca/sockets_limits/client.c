#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define SOCKNAME "/tmp/socket99"

int main() {
	int local_server_sock;
	struct sockaddr_un local_server;

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

	printf("Connected!!\n");
	//------------------------------------------------------------------------------------------------------------------------------------
	char *buff;
	size_t size = 0;
	getline(&buff, &size, stdin);

	printf("i will send %s\n, size=%ld\n", buff, sizeof(size));

	size = strlen(buff) + 1;

	send(local_server_sock, (void *)&size, sizeof(size), 0);

	send(local_server_sock, (void *)buff, size, 0);

	return 0;
}