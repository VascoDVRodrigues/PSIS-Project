#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BOLDWHITE "\033[1m\033[37m" /* Bold White */

#include "KVS-lib.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *func(void *arg) {
	char group_id[1024];
	char secret[1024];

	char *result;
	int a;

	while (1) {
		// critical region so the scanfs dont get super confuse
		pthread_mutex_lock(&lock);
		printf(BOLDWHITE "t: %ld, Enter GroupId: ", pthread_self());
		scanf("%s", group_id);
		printf("t: %ld, Enter Secret: ", pthread_self());
		scanf("%s", secret);
		pthread_mutex_unlock(&lock);

		// establishes connection with the local server
		a = establish_connection(group_id, secret);
		if (a == 0) {
			printf("Established conNEECtion ✔️\n");
			break;
		} else if (a == WRONG_SECRET) {
			printf("Correct Group, wrong password ❌\n");
		} else if (a == WRONG_GROUP) {
			printf("No Group with same GroupId ❌\n");
		} else if (a == ERROR_CONNECTION) {
			printf("Error in connection to server ❌\n");
		} else if (a == FULL_SERVER) {
			printf("Full server, try again later 👥\n");
		} else if (a == TIMED_OUT) {
			printf("Timed out ⌛️\n");
		}
	}
	sleep(5);
	a = close_connection();
	if (a == NO_ERROR) {
		printf("Connection closed ✔️\n");
	} else {
		printf("Error ❌\n");
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

	pthread_t t1, t2, t3, t4;
	pthread_create(&t1, NULL, func, NULL);
	// usleep(500);
	pthread_create(&t2, NULL, func, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	close_connection();

	return 0;
}