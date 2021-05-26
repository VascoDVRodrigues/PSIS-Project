#include "linkedList-lib.h"

SubNode *create_LinkedList() {
	SubNode *head = NULL;
	return head;
}

int isEmpty(SubNode *node) {
	if (node == NULL) {
		return 1;
	}
	return 0;
}

SubNode *insertNode(char *key, char *value, SubNode *head) {
	SubNode *node = (SubNode *)malloc(sizeof(SubNode));
	node->key = (char *)malloc(100);
	node->value = (char *)malloc(100);
	strcpy(node->key, key);
	strcpy(node->value, value);
	node->next = head;
	//head = node;
	return node;
}

SubNode *searchNode(char *key, SubNode *head) {
	if (isEmpty(head)) {
		return head;
	}

	SubNode *current = head;
	while (current != NULL) {
		if (strcmp(current->key, key) == 0) {  // Encontramos o nó
			return current;
		}
		current = current->next;
	}
	return NULL;
}

SubNode *deleteNode(char *key, SubNode *head) {
	SubNode *current_node = head;
	SubNode *aux_node = NULL;
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
				return head;
			}
			current_node = current_node->next;
		}
	}
	return head;
}

void printList(SubNode *head) {
	SubNode *current = head;
	printf("HEAD->");
	while (current != NULL) {
		printf("%s|%s", current->key, current->value);
		current = current->next;
		printf("-->\n");
	}
	printf("NULL\n");
}

void printNode(SubNode *node) {
	printf("SubNode-> %s|%s\n", node->key, node->value);
	return;
}

void updateValue(SubNode *node, char *newValue) {
	strcpy(node->value, newValue);
	return;
}