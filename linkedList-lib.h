#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

typedef struct node {
	char *key;
	char *value;
	struct node *next;
} Node;

Node *create_LinkedList();

Node *insertNode(char *key, char *value, Node *head);

Node *searchNode(char *key, Node *head);

Node *deleteNode(char *key, Node *head);

int isEmpty(Node *node);

void printList(Node *head);

void printNode(Node *node);

void updateValue(Node *node, char *newValue);