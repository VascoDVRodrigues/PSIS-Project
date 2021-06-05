#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

#include "KVS-lib.h"

void keyALterada(char *key) {
	printf("\n\t %s was altered. This is the callback function.\n", key);

	// put_value(key, "some new key");

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
			printf(BOLDWHITE "Enter GroupId: ");
			scanf("%s", group_id);
			printf("Enter Secret: ");
			scanf("%s", secret);
			a = establish_connection(group_id, secret);
			if (a == 0) {
				printf("Established conNEECtion ✔️\n");
				break;
			} else if (a == -1) {
				printf("Correct Group, wrong password ❌\n");
			} else if (a == -2) {
				printf("No Group with same GroupId ❌\n");
			} else if (a == -3) {
				printf("Error in connection to server ❌\n");
			} else if (a == -5) {
				printf("Full server, try again later 👥\n");
			} else if (a == -4) {
				printf("Timed out ⌛️\n");
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
					printf("Value stored ✔️\n");
				} else if (a == -2) {
					printf("Grupo foi apagado ❌\n");
					break;
				} else {
					printf("Error ❌\n");
				}
			} else if (strcmp(option, "get") == 0) {
				a = get_value(group_id, &result);
				if (a == 0) {
					printf("Value retrieved ✔️\n");
					printf("Value: %s\n", result);
				} else if (a == -2) {
					printf("Grupo foi apagado ❌\n");
					break;
				} else {
					printf("Error ❌\n");
				}
			} else if (strcmp(option, "delete") == 0) {
				a = delete_value(group_id);
				if (a == 0) {
					printf("Value deleted ✔️\n");
				} else {
					printf("Error ❌\n");
				}
			} else if (strcmp(option, "close") == 0) {
				a = close_connection();
				if (a == 0) {
					printf("Connection closed ✔️\n");
				} else {
					printf("Error ❌\n");
				}
				break;
			} else if (strcmp(option, "register") == 0) {
				a =register_callback(group_id, &keyALterada);
				if (a == 0) {
					printf("Callback registered ✔️\n");
				} else {
					printf("Error ❌\n");
				}
			}
		}
	}
	return 0;
}