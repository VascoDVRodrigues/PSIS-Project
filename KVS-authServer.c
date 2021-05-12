#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

typedef struct package {
	char key[1024];
	char value[1024];
} Package;

#define MAX_CONNECTIONS 10

int func() {}

int main() {
	pthread_t local_servers[MAX_CONNECTIONS];
	struct sockaddr_in endereco;
	int fd;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	endereco.sin_family = AF_INET;
	endereco.sin_port = htons(22);
	endereco.sin_addr.s_addr = INADDR_ANY;
	bind(fd, (const struct sockaddr *)&endereco, sizeof(endereco));

	return 0;
}