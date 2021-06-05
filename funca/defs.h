#include <arpa/inet.h>
#include <fcntl.h> /* For O_* constants */
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h> /* For mode constants */
#include <sys/types.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#include "linkedList-lib.h"

#define SOCKNAME "/tmp/socket1"

#define CALLBACKS_SOCK "/tmp/socket99"

#define AUTHSERVER_IP "127.0.0.1"

#define AUTHSERVER_TIMEOUT 5  // timeout of 5 seconds

#define MAX_CONNECTIONS 2

#define RESET "\033[0m"
#define BOLDGREEN "\033[1m\033[32m" /* Bold Green */
#define BOLDBLUE "\033[1m\033[34m"	/* Bold Blue */
#define BOLDWHITE "\033[1m\033[37m" /* Bold White */

// Package used in comunications between local and auth server
typedef struct auth_package {
	char groupID[1024];
	char secret[1024];
	int mode;
} Package;

// Struct with info about an INET socket
typedef struct _server_info_pack_inet {
	int socket;
	struct sockaddr_in adress;
} Server_info_pack_INET;

// Struct with info about an UNIX socket
typedef struct _server_info_pack {
	int socket;
	struct sockaddr_un adress;
} Server_info_pack;

// Struct to save info about a specific group
typedef struct _group {
	LinkedList* keyValue_List;
	char groupID[1024];
	int n_keyValues;
} Grupo;

// Struct with info about a client
typedef struct _client_info {
	int socket;
	int callback_sock_index;
	int index_client_thread_status;
	Grupo* connected_group;
} Client_info;

// Struct with info about a key|value used in the local server
typedef struct _keyValue {
	void (*callback_func)(char*);
	char key[128];
	char* value;
} KeyValue;

// Struct with info about a app that was/is connected to the local server
typedef struct _appsInfo {
	int PID, connected;
	time_t start, end;
} AppInfo;
