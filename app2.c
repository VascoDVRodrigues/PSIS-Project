#include "KVS-lib.h"

int main() {
	char group_id[100];
	char secret[100];
	char option[20];
	char *result;
	int a;
	while (1) {
		printf("Enter GroupId: ");
		scanf("%s", group_id);
		printf("Enter Secret: ");
		scanf("%s", secret);
		a = establish_connection(group_id, secret);
		if (a == 0) {
			printf("Established connection\n");
			break;
		} else if (a == -2) {
			printf("No Group with same GroupId! :(\n");
		} else if (a == -3) {
			printf("Wrong Key! :(\n");
		}
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
			printf("Value: %s\n", result);
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
			} else {
				printf("nao funca\n");
			}
			exit(1);
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