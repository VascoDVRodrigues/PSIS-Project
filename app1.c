#include "KVS-lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCKNAME "/tmp/socket1"

int main()
{
    int sock;
    struct sockaddr_un server;
    char buf[1024];
    char str[100];

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Error creating socket");
        exit(1);
    }
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCKNAME);

    if (connect(sock, (struct sockaddr *)&server, sizeof(struct sockaddr_un)) < 0)
    {
        close(sock);
        perror("connecting stream socket");
        exit(1);
    }
    strcpy(buf, "DATA");
    if (send(sock, buf, sizeof(buf), 0) < 0)
    {
        perror("writing on stream socket");
    }

    int n = recv(sock, str, 100, 0);

    printf("Received %d bytes, message: %s\n", n, str);

    close(sock);

    return 0;
}