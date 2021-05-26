#include "linkedList-lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

struct _LinkedList {
	Item item;
	struct _LinkedList *next;
};

LinkedList *createList(void) { return NULL; }

LinkedList *insertNode(LinkedList *head, Item data) {
	LinkedList *node = (LinkedList *)malloc(sizeof(LinkedList));
	node->item = data;
	node->next = head;
	return node;
}

Item *searchNode(LinkedList *head, Item data_to_find, int compareItem(Item, Item)) {
	if (head == NULL) {
		return NULL;
	}

	LinkedList *current = head;

	while (current != NULL) {
		if (compareItem(data_to_find, current->item) == 1) {  // found
			printf("found\n");
			return current->item;
		}
		current = current->next;
	}
	return NULL;
}

LinkedList *updateNode(LinkedList *head, Item data_to_update, Item update, int compareItem(Item, Item), void freeItem(Item)) {
	if (head == NULL) {
		return head;
	}

	LinkedList *current = head;

	while (current != NULL) {
		if (compareItem(data_to_update, current->item) == 1) {	// found
			freeItem(current->item);							// apagar o item atual
			// adicionar o item a atualizar
			// NAO se apaga os apontadores
			current->item = update;
			return head;
		}
		current = current->next;
	}
	return head;  // retorna-se a cabeca pois se a data a dar update nao existir e se for retornado null depois perde-se a lista completa
}

LinkedList *deleteNode(LinkedList *head, Item data_to_delete, int compareItem(Item, Item), void freeItem(Item)) {
	LinkedList *current_node = head;
	LinkedList *aux_node = NULL;

	if (compareItem(head->item, data_to_delete)) {
		head = current_node->next;
		freeItem(current_node->item);
		free(current_node);
		return head;
	} else {
		while (current_node->next != NULL) {
			if (compareItem(current_node->next->item, data_to_delete)) {
				aux_node = current_node->next;
				current_node->next = current_node->next->next;
				freeItem(aux_node->item);
				free(aux_node);
				return head;
			}
			current_node = current_node->next;
		}
	}
	return head;
}

int numItens(LinkedList *head) {
	LinkedList *aux; /* auxiliar pointers to travel through the list */
	int count = 0;
	aux = head;

	for (aux = head; aux != NULL; aux = aux->next) {
		count++;
	}

	return count;
}

void printList(LinkedList *head, void printItem(Item), int tabs) {
	if (head == NULL) {
		printf("Empty List\n");
		return;
	}

	LinkedList *current = head;
	printf("\tHEAD->\n");
	while (current != NULL) {
		printItem(current->item);
		current = current->next;
		for (int i = 0; i < tabs; i++) {
			printf("\t");
		}
		printf("-->\n");
	}
	printf("\tNULL\n");
}

LinkedList *clearList(LinkedList *head, void freeItem(Item)) {
	LinkedList *aux, *newhead; /* auxiliar pointers to travel through the list */

	for (aux = head; aux != NULL; aux = newhead) {
		newhead = aux->next;
		freeItem(aux->item);
		free(aux);
	}
	return aux;
}