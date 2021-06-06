#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BOLDWHITE "\033[1m\033[37m" /* Bold White */

#include "KVS-lib.h"

/*
 * Function: randomString
 * ----------------------------
 *   Generates a random string with length size
 *
 *   size: length of the string to generate
 *
 *   returns: Generated string
 *
 */
char *randomString(int size) {
	char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890\0";

	char *randString = calloc(size, sizeof(char));

	for (int i = 0; i < size; i++) {
		randString[i] = chars[rand() % strlen(chars)];
	}

	randString[size + 1] = '\0';
	return randString;
}

void *func(void *arg) {
	int a;
	char keys[30][5];
	for (int i = 0; i < 5; i++) {
		strcpy(keys[i], randomString(4));
		a = put_value(keys[i], randomString(6));
		if (a == 0) {
			printf("\tValue stored ✔️\n");
		} else if (a == -2) {
			printf("\tGrupo foi apagado ❌\n");
			pthread_exit((void *)1);
		} else {
			printf("\tError ❌\n");
			pthread_exit((void *)1);
		}
	}
}

void *func1(void *arg) {
	int a;
	char keys[30][5];
	for (int i = 0; i < 5; i++) {
		strcpy(keys[i], randomString(4));
		a = put_value(keys[i], randomString(6));
		if (a == 0) {
			printf("Value stored ✔️\n");
		} else if (a == -2) {
			printf("Grupo foi apagado ❌\n");
			pthread_exit((void *)1);
		} else {
			printf("Error ❌\n");
			pthread_exit((void *)1);
		}
	}
	char *result;
	for (int i = 4; i >= 0; i--) {
		a = get_value(keys[i], &result);
		if (a == NO_ERROR) {
			printf("Value retrieved ✔️\n");
			printf("Value: %s\n", result);
		} else if (a == GROUP_DELETED) {
			printf("Group was deleted ❌\n");
			break;
		} else if (a == NO_KEY) {
			printf("Key doesn't exist ❌\n");
		} else if (a == ERROR_CONNECTION) {
			printf("Error comunicating with the server ❌\n");
			break;
		} else {
			printf("Error ❌\n");
		}
	}
}

// exemple of function to be executed whenever a key's value is updated
void keyALterada(char *key) {
	printf("\n\t %s was altered. This is the callback function.\n", key);

	// put_value(key, "some new key");

	return;
}

int main() {
	// Comment this line to generate the same strings every time
	srand(time(NULL));

	char group_id[1024];
	char secret[1024];

	char *result;
	int a;

	while (1) {
		printf(BOLDWHITE "Enter GroupId: ");
		scanf("%s", group_id);
		printf("Enter Secret: ");
		scanf("%s", secret);

		// establishes connection with the local server
		a = establish_connection(group_id, secret);
		if (a == 0) {
			printf("Established conNEECtion ✔️\n");
			break;
		} else if (a == -1) {
			printf("Correct Group, wrong password ❌\n");
			;
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

	pthread_t t1, t2, t3, t4;
	pthread_create(&t1, NULL, func, NULL);
	//usleep(500);
	pthread_create(&t2, NULL, func1, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	close_connection();

	return 0;
}