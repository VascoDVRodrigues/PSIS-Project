#include <unistd.h>

#include "KVS-lib.h"

void keyALterada(char *key) {
	printf("\n\tA key %s foi alterada\n\tEsta é func de callback\n", key);

	// put_value(key, "o bruno e bue gay");

	return;
}

int main() {
	char group_id[1024];
	char secret[1024];

	char key[128];
	char value[5000];  // infinito

	char option[20];
	char *result;
	int a;
	while (1) {
		while (1) {
			printf("Enter GroupId: ");
			scanf("%s", group_id);
			printf("Enter Secret: ");
			scanf("%s", secret);
			a = establish_connection(group_id, secret);
			printf("ret do establish: %d\n", a);
			if (a == 0) {
				printf("Established conNEECtion\n");
				break;
			} else if (a == -1) {
				printf("Correct Group, wrong password... :(\n");
			} else if (a == -2) {
				printf("No Group with same GroupId! :(\n");
			} else if (a == -3) {
				printf("Wrong Key! :(\n");
			} else if (a == -5) {
				printf("Full server, try again later!\n");
			} else if (a == -6) {
				printf("Timed out\n");
			}
		}
		char *line;
		size_t n;
		while (1) {
			printf("Enter option: ");
			usleep(100);
			scanf("%s %s", option, group_id);
			getchar();	// para ler o \n
			if (strcmp(option, "put") == 0) {
				printf("Value to store: ");
				int x = getline(&line, &n, stdin);
				a = put_value(group_id, line);
				if (a == 0) {
					printf("funca\n");
				} else if (a == -4) {
					printf("Grupo foi apagado\n");
					break;
				} else {
					printf("nao funca\n");
				}
			} else if (strcmp(option, "get") == 0) {
				a = get_value(group_id, &result);
				if (a == 0) {
					printf("funca\n");
					printf("Value: %s\n", result);
				} else if (a == -4) {
					printf("Grupo foi apagado\n");
					break;
				} else {
					printf("nao funca\n");
				}
			} else if (strcmp(option, "delete") == 0) {
				a = delete_value(group_id);
				if (a == 0) {
					printf("funca\n");
				} else {
					printf("nao funca\n");
				}
			} else if (strcmp(option, "close") == 0) {
				a = close_connection();
				if (a == 0) {
					printf("funca\n");
				} else if (a == -4) {
					printf("Grupo foi apagado\n");
					break;
				} else {
					printf("nao funca\n");
				}
				break;
			} else if (strcmp(option, "register") == 0) {
				register_callback(group_id, &keyALterada);
			}
		}
	}

	/*
	char *value;
	a = get_value("1234", &value);
	if (a == 0)
	{
		printf("Received %s\n", value);
	}
	else
	{
		printf("nao funca");
	}
*/
	// close_connection();
	return 0;
}