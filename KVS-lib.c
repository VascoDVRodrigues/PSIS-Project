#include <unistd.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SOCKNAME "/tmp/socket1"

int local_server_sock;
struct sockaddr_un local_server;

int establish_connection(char *group_id, char *secret)
{
    local_server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (local_server_sock < 0)
    {
        perror("Error creating socket");
        return -1;
    }
    local_server.sun_family = AF_UNIX;
    strcpy(local_server.sun_path, SOCKNAME);

    if (connect(local_server_sock, (struct sockaddr *)&local_server, sizeof(struct sockaddr_un)) < 0)
    {
        close(local_server_sock);
        perror("connecting stream socket");
        return -2;
    }
    printf("Connected to local server\n");
    return 0;
}
int put_value(char *key, char *value)
{
}
int get_value(char *key, char **value)
{
}
int delete_value(char *key)
{
}
int register_callback(char *key, void (*callback_function)(char *))
{
}
int close_connection()
{
    printf("Hello World\n");
}