#include "KVS-lib.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char group_id[100];
    char secret[100];
    int a = establish_connection(group_id, secret);
    if (a == 0)
    {
        printf("Established connection\n");
    }
    else
    {
        printf("nao funca");
    }
    close_connection();

    return 0;
}