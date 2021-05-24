#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKNAME "/tmp/socket1"

#define MESSAGE_SIZE 100
#define MAX_CONNECTIONS 10

typedef struct auth_package {
	char groupID[1024];
	char secret[1024];
	int mode;
} Auth_Package;

typedef struct package {
	int mode;
	char key[MESSAGE_SIZE];
	char value[MESSAGE_SIZE];
} Package;

typedef struct _server_info_pack {
	int socket;
	struct sockaddr_in adress;
} Server_info_pack;
