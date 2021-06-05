#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BOLDWHITE "\033[1m\033[37m"	  /* Bold White */

#include "KVS-lib.h"

//exemple of function to be executed whenever a key's value is updated
void keyALterada(char *key) {
	printf("\n\t %s was altered. This is the callback function.\n", key);

	// put_value(key, "some new key");

	return;
}

int main() {
	char group_id[1024];
	char secret[1024];

	char key[128];

	//the value is defined for demonstration purposes, but the system can handle bigger sized files
	char value[5000];  

	char option[20];
	char *result;
	int a;
	char *line;
	size_t n;

	while (1) {
		while (1) {
			printf(BOLDWHITE "Enter GroupId: ");
			scanf("%s", group_id);
			printf("Enter Secret: ");
			scanf("%s", secret);

			//establishes connection with the local server
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
		//the app reaches this point whenever there is a successfull connection with one of the groups
		while (1) {
			printf("Enter option: ");
			//we need this sleep for the callback function results not to overlap and destroy the app UI
			usleep(100);
			scanf("%s %s", option, group_id);
			getchar();	

			//Put value on the local server
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
			}//get value from the local server 
			else if (strcmp(option, "get") == 0) {
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
			} //Delete value from the local server
			else if (strcmp(option, "delete") == 0) {
				a = delete_value(group_id);
				if (a == 0) {
					printf("Value deleted ✔️\n");
				} else if (a == -2) {
					printf("Grupo foi apagado ❌\n");
					break;
				} else {
					printf("Error ❌\n");
				}
			} //Close the connection with the local server
			else if (strcmp(option, "close") == 0) {
				a = close_connection();
				if (a == 0) {
					printf("Connection closed ✔️\n");
				} else {
					printf("Error ❌\n");
				}
				break;
			} //Register a callback on a key
			else if (strcmp(option, "register") == 0) {
				a = register_callback(group_id, &keyALterada);
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