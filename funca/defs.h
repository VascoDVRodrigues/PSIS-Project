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

// #define MESSAGE_SIZE 100
#define MAX_CONNECTIONS 2

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

typedef struct auth_package {
	char groupID[1024];
	char secret[1024];
	int mode;
} Package;

typedef struct _server_info_pack_inet {
	int socket;
	struct sockaddr_in adress;
} Server_info_pack_INET;

typedef struct _server_info_pack {
	int socket;
	struct sockaddr_un adress;
} Server_info_pack;

// Structs que estao na lista de listas
typedef struct _group {
	LinkedList* keyValue_List;
	// Talvez aqui por malloc
	char groupID[1024];
	int n_keyValues;
	int deleted;	// se foi apagado
	int connected;	// se existe alguma app ligada a este grupo
} Grupo;

// struct enviada ao client handler
typedef struct _client_info {
	int socket;
	int callback_sock_index;
	// guarda o indice correspondente a thread no vetor dos status das threads
	int index_client_thread_status;
	Grupo* connected_group;
} Client_info;

// Struct que guarda o key|value
typedef struct _keyValue {
	void (*callback_func)(char*);
	char key[128];
	char* value;
} KeyValue;

typedef struct _appsInfo {
	int PID, connected;
	// para guadar o tempo fazer time(&start),
	// para dar print fazer ctime_r(&start);
	// NÃO USAR A FUNC ctime_r pq nao é thread safe
	// ou então usar a funcao strftime(...) para formatar o output
	time_t start, end;
} AppInfo;
