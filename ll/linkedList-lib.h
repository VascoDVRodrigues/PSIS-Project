#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

typedef struct _node {
	char *key;
	char *value;
	struct _node *next;
} SubNode;

typedef struct _main_node {
	struct _node *GroupHead;
	struct _main_node *next;
} MainNode;

SubNode *create_LinkedList();

SubNode *insertNode(char *key, char *value, SubNode *head);

SubNode *searchNode(char *key, SubNode *head);

SubNode *deleteNode(char *key, SubNode *head);

int isEmpty(SubNode *node);

void printList(SubNode *head);

void printNode(SubNode *node);

void updateValue(SubNode *node, char *newValue);