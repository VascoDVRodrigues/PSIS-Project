#include <unistd.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

typedef struct node
{
    char *key;
    char *value;
    struct node *next;
} Node;

Node *create_LinkedList();

Node *insertNode(char *key, char *value, Node *head);

Node *searchNode(char *key, Node *head);

void deleteNode(char *key, Node *head);

int isEmpty(Node *node);

void printList(Node *head);

void printNode(Node *node);

void updateValue(Node *node, char *newValue);