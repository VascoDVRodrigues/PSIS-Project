#include "linkedList-lib.h"

Node *create_LinkedList() {
	Node *head = NULL;
	return head;
}

int isEmpty(Node *node) {
	if (node == NULL) {
		return 1;
	}
	return 0;
}

Node *insertNode(char *key, char *value, Node *head) {
	Node *node = (Node *)malloc(sizeof(Node));
	node->key = (char *)malloc(100);
	node->value = (char *)malloc(100);
	strcpy(node->key, key);
	strcpy(node->value, value);
	node->next = head;
	head = node;
	return node;
}

Node *searchNode(char *key, Node *head) {
	if (isEmpty(head)) {
		return head;
	}

	Node *current = head;
	while (current != NULL) {
		if (strcmp(current->key, key) == 0)	 // Encontramos o nó
		{
			return current;
		}
		current = current->next;
	}
}

Node *deleteNode(char *key, Node *head) {
	Node *current_node = head;
	Node *aux_node = NULL;
	if (strcmp(head->key, key) == 0) {
		head = head->next;
		free(current_node);
		return head;
	} else {
		while (current_node->next != NULL) {
			if (strcmp(current_node->next->key, key) == 0) {
				aux_node = current_node->next;
				current_node->next = current_node->next->next;
				free(aux_node);
			}
			current_node = current_node->next;
		}
	}
	return head;
}

void printList(Node *head) {
	Node *current = head;
	while (current != NULL) {
		printf("Key: %s, Value: %s\n", current->key, current->value);
		current = current->next;
	}
}

void printNode(Node *node) {
	printf("Node-> Key: %s\tValue: %s\n", node->key, node->value);
	return;
}

void updateValue(Node *node, char *newValue) {
	strcpy(node->value, newValue);
	return;
}