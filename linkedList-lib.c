#include "linkedList-lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

/* type definition for structure to hold list item */
struct _LinkedList {
	Item item;
	struct _LinkedList *next;
};

LinkedList *createList(void) { return NULL; }

/*
 * Function: insertNode
 * ----------------------------
 *   Inserts a node at the beginning
 *
 *   data: pointer to data to add
 *	 head: pointer to the head of the list to insert the new node
 *
 *   returns: Head of the list with the new node inserted
 *
 */
LinkedList *insertNode(LinkedList *head, Item data) {
	LinkedList *node = (LinkedList *)malloc(sizeof(LinkedList));
	node->item = data;
	node->next = head;
	return node;
}

/*
 * Function: searchNode
 * ----------------------------
 *   Searches a node inside a certain linked list
 *
 *   data_to_find: pointer to data to find
 *	 compareItem: function that compares two items
 *	 head: pointer to the head of the list
 *
 *   returns: The item if found
 * 			  NULL otherwise
 *
 */
Item *searchNode(LinkedList *head, Item data_to_find, int compareItem(Item, Item)) {
	if (head == NULL) {
		return NULL;
	}

	LinkedList *current = head;

	while (current != NULL) {
		if (compareItem(data_to_find, current->item) == 1) {  // found
			return current->item;
		}
		current = current->next;
	}
	return NULL;
}

/*
 * Function: updateNode
 * ----------------------------
 *   Updates a node inside a certain linked list
 *
 *   data_to_update: pointer to data to update
 * 	 update: pointer to data to replace the previous node
 *	 compareItem: function that compares two items
 * 	 freeItem: function that frees a item
 *	 head: pointer to the head of the list
 *
 *   returns: Head of the list with the update
 *
 */
LinkedList *updateNode(LinkedList *head, Item data_to_update, Item update, int compareItem(Item, Item), void freeItem(Item)) {
	if (head == NULL) {
		return head;
	}

	LinkedList *current = head;

	while (current != NULL) {
		if (compareItem(data_to_update, current->item) == 1) {	// found
			freeItem(current->item);							// delete current item
			// add the update
			// do not touch the pointers
			current->item = update;
			return head;
		}
		current = current->next;
	}
	return head;
}

/*
 * Function: deleteNode
 * ----------------------------
 *   Deletes a node inside a certain linked list
 *
 *   data_to_delete: pointer to data to delete
 *	 compareItem: function that compares two items
 * 	 freeItem: function that frees a item
 *	 head: pointer to the head of the list
 *
 *   returns: Head of the list with the update
 *
 */
LinkedList *deleteNode(LinkedList *head, Item data_to_delete, int compareItem(Item, Item), void freeItem(Item)) {
	if (head == NULL) {
		return head;
	}

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

/*
 * Function: printList
 * ----------------------------
 *   Calls the function that prints a item to every item in the list
 *
 *   head: pointer to the head of the list
 *	 printItem: function that prints a item
 *	 tabs: number of tabs to be placed before the print of the item
 *
 */
void printList(LinkedList *head, void printItem(Item), int tabs) {
	if (head == NULL) {
		printf("Nothing to see here, this list is empty... :P\n");
		return;
	}

	LinkedList *current = head;
	// printf("\tHEAD->\n");
	while (current != NULL) {
		printItem(current->item);
		current = current->next;
		for (int i = 0; i < tabs; i++) {
			printf("\t");
		}
		printf("\n");
	}
	// printf("\tNULL\n");
}

/*
 * Function: clearList
 * ----------------------------
 *   Deletes an entire linked list
 *
 *   head: pointer to the head of the list
 *	 freeItem: function that frees a item
 *
 *	 Returns: pointer to NULL
 */
LinkedList *clearList(LinkedList *head, void freeItem(Item)) {
	LinkedList *aux, *newhead; /* auxiliar pointers to travel through the list */

	for (aux = head; aux != NULL; aux = newhead) {
		newhead = aux->next;
		freeItem(aux->item);
		free(aux);
	}
	return aux;
}