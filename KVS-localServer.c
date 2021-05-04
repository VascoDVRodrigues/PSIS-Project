#include <unistd.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SOCKNAME "/tmp/socket1"
/* ERROR CODE:

*/
int main()
{
    char str[100];
    struct sockaddr_un server;
    int send_socket;
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCKNAME);

    unlink(SOCKNAME);

    send_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (send_socket < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    if (bind(send_socket, (struct sockaddr *)&server, sizeof(struct sockaddr_un)))
    {
        perror("Error binding stream socket");
        exit(1);
    }

    printf("Socket created, has name %s\n", server.sun_path);

    while(1) {
        listen(send_socket, 10);
        printf("Waiting for connections!!\n");
        int client = accept(send_socket, NULL, NULL);

        if (client != -1)
        {
            printf("Connected to %d\n", client);
        }

        int n = recv(client, str, 100, 0);

        printf("Received %d bytes, message: %s\n", n, str);

        send(client, str, n, 0);
    }

    close(send_socket);
    unlink(SOCKNAME);
}