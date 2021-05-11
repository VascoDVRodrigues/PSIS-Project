#include "KVS-lib.h"

int main() {
	char group_id[100];
	char secret[100];
	char option[20];
	char *result;
	strcpy(group_id, "1234");
	strcpy(secret, "olá");
	int a = establish_connection(group_id, secret);
	if (a == 0) {
		printf("Established connection\n");
	} else {
		printf("nao funca");
	}
	while (1) {
		printf("Enter option: ");
		scanf("%s %s %s", option, group_id, secret);
		if (strcmp(option, "put") == 0) {
			a = put_value(group_id, secret);
			if (a == 0) {
				printf("funca\n");
			} else {
				printf("nao funca\n");
			}
		} else if (strcmp(option, "get") == 0) {
			a = get_value(group_id, &result);
			if (a == 0) {
				printf("funca\n");
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