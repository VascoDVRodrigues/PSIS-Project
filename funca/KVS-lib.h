#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

int establish_connection(char *group_id, char *secret);
int put_value(char *key, char *value);
int get_value(char *key, char **value);
int delete_value(char *key);
int register_callback(char *key, void (*callback_function)(char *));
int close_connection();